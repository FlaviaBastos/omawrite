#include "previewdocument.h"

#include <QFileInfo>
#include <QHash>
#include <QImage>
#include <QImageReader>
#include <QRegularExpression>
#include <QTextFormat>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextList>

namespace {
constexpr qreal previewLineHeightPercent = 140;

QString canonicalOrEmpty(const QString &path)
{
    const QString canonical = QFileInfo(path).canonicalFilePath();
    return canonical;
}
}

PreviewDocument::PreviewDocument(std::function<void(const QString &)> imageLoaded, QObject *parent)
    : QTextDocument(parent)
    , m_imageLoaded(std::move(imageLoaded))
{
}

void PreviewDocument::setImageRoot(const QString &root)
{
    m_imageRoot = canonicalOrEmpty(root);
}

bool PreviewDocument::setImageWidth(int width)
{
    const int imageWidth = qMax(1, width);
    if (m_imageWidth == imageWidth)
        return false;
    m_imageWidth = imageWidth;
    return true;
}

void PreviewDocument::setAllowedImages(const QString &markdown, const QUrl &baseUrl)
{
    m_allowedImages.clear();
    static const QRegularExpression imageRe(
        QStringLiteral("!\\[[^\\]]*\\]\\((?:<([^>]+)>|([^\\s)]+))(?:\\s+[^)]*)?\\)"));
    QRegularExpressionMatchIterator matches = imageRe.globalMatch(markdown);
    while (matches.hasNext()) {
        const QRegularExpressionMatch match = matches.next();
        allowImage(match.captured(1).isEmpty() ? match.captured(2) : match.captured(1), baseUrl);
    }

    QHash<QString, QString> references;
    static const QRegularExpression referenceRe(
        QStringLiteral("^\\s{0,3}\\[([^\\]]+)\\]:\\s*(?:<([^>]+)>|([^\\s]+))"),
        QRegularExpression::MultilineOption);
    QRegularExpressionMatchIterator definitions = referenceRe.globalMatch(markdown);
    while (definitions.hasNext()) {
        const QRegularExpressionMatch match = definitions.next();
        references.insert(referenceKey(match.captured(1)),
                          match.captured(2).isEmpty() ? match.captured(3) : match.captured(2));
    }

    static const QRegularExpression referenceImageRe(
        QStringLiteral("!\\[([^\\]]*)\\]\\[([^\\]]*)\\]"));
    matches = referenceImageRe.globalMatch(markdown);
    while (matches.hasNext()) {
        const QRegularExpressionMatch match = matches.next();
        const QString destination = references.value(referenceKey(
            match.captured(2).isEmpty() ? match.captured(1) : match.captured(2)));
        if (!destination.isEmpty())
            allowImage(destination, baseUrl);
    }
}

void PreviewDocument::applyTypography()
{
    const QFont font = defaultFont();
    const qreal pixelSize = font.pixelSize() > 0
        ? font.pixelSize()
        : (font.pointSizeF() > 0 ? font.pointSizeF() * 96.0 / 72.0 : 20.0);
    const qreal paraGap = pixelSize * 0.75;

    QTextCursor cursor(this);
    cursor.beginEditBlock();
    for (QTextBlock block = begin(); block.isValid(); block = block.next()) {
        QTextBlockFormat format = block.blockFormat();
        format.setLineHeight(previewLineHeightPercent, QTextBlockFormat::ProportionalHeight);

        const int heading = format.headingLevel();
        const int quoteLevel = format.intProperty(QTextFormat::BlockQuoteLevel);
        if (heading > 0) {
            format.setTopMargin(paraGap * (heading == 1 ? 1.6 : 1.2));
            format.setBottomMargin(paraGap * 0.4);
        } else if (block.textList()) {
            format.setTopMargin(0);
            format.setBottomMargin(0);
        } else if (quoteLevel > 0) {
            format.setTopMargin(paraGap * 0.5);
            format.setBottomMargin(paraGap * 0.5);
        } else {
            format.setTopMargin(0);
            format.setBottomMargin(paraGap);
        }

        cursor.setPosition(block.position());
        cursor.mergeBlockFormat(format);
    }
    cursor.endEditBlock();
}

QVariant PreviewDocument::loadImageResource(const QUrl &url)
{
    return loadResource(QTextDocument::ImageResource, url);
}

QVariant PreviewDocument::loadResource(int type, const QUrl &url)
{
    // Never hand unknown resource types to QTextDocument: its default loader
    // will fetch remote URLs. Images are the only resource this preview loads.
    if (type != QTextDocument::ImageResource)
        return {};

    QUrl resolved = url;
    if (url.isRelative())
        resolved = baseUrl().resolved(url);
    if (!resolved.isLocalFile())
        return {};

    const QString localFile = resolved.toLocalFile();
    const QString path = canonicalOrEmpty(localFile);
    if (path.isEmpty() || m_imageRoot.isEmpty() || !pathIsInsideRoot(path)
            || !m_allowedImages.contains(localFile)
            || !QFileInfo(path).isFile()) {
        return {};
    }

    // SVG can pull in external resources; refuse it rather than denylisting
    // individual tags.
    if (path.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive))
        return {};

    QImageReader::setAllocationLimit(32);
    QImageReader reader(path);
    const QSize size = reader.size();
    if (!size.isValid())
        return {};
    const int maxEdge = qMax(m_imageWidth, 1);
    if (size.width() > maxEdge || size.height() > maxEdge * 4)
        reader.setScaledSize(size.scaled(maxEdge, maxEdge * 4, Qt::KeepAspectRatio));

    const QImage image = reader.read();
    if (image.isNull())
        return {};

    if (m_imageLoaded)
        m_imageLoaded(path);
    return image;
}

void PreviewDocument::allowImage(const QString &destination, const QUrl &baseUrl)
{
    const QUrl source(destination);
    if (!source.isRelative() || !source.scheme().isEmpty())
        return;

    const QString localFile = baseUrl.resolved(source).toLocalFile();
    const QString canonical = canonicalOrEmpty(localFile);
    if (canonical.isEmpty() || !pathIsInsideRoot(canonical))
        return;

    m_allowedImages.insert(localFile);
}

QString PreviewDocument::referenceKey(const QString &label)
{
    return label.simplified().toCaseFolded();
}

bool PreviewDocument::pathIsInsideRoot(const QString &canonicalPath) const
{
    if (canonicalPath == m_imageRoot)
        return false;
    return canonicalPath.startsWith(m_imageRoot + QLatin1Char('/'));
}
