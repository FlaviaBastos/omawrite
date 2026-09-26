#pragma once

#include <QSet>
#include <QString>
#include <QTextDocument>
#include <QUrl>
#include <functional>

class PreviewDocument final : public QTextDocument {
public:
    explicit PreviewDocument(std::function<void(const QString &)> imageLoaded,
                             QObject *parent = nullptr);

    void setImageRoot(const QString &root);
    bool setImageWidth(int width);
    void setAllowedImages(const QString &markdown, const QUrl &baseUrl);
    void applyTypography();

    // Test hook: exercises the same loader the renderer uses.
    QVariant loadImageResource(const QUrl &url);

protected:
    QVariant loadResource(int type, const QUrl &url) override;

private:
    void allowImage(const QString &destination, const QUrl &baseUrl);
    static QString referenceKey(const QString &label);
    bool pathIsInsideRoot(const QString &canonicalPath) const;

    std::function<void(const QString &)> m_imageLoaded;
    QString m_imageRoot;
    QSet<QString> m_allowedImages;
    int m_imageWidth = 800;
};
