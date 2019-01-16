//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (c) 2019 mirabilos
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#include "audioattribution.h"

namespace Ms {

//---------------------------------------------------------
//   AudioAttribution
//---------------------------------------------------------

AudioAttribution::AudioAttribution(Score* score)
      {
      QJsonObject toplev;
      QJsonObject producer;
      QJsonObject scoreobj;
      QJsonObject scoremeta;

      producer.insert(QString("name"), QJsonValue("MuseScore"));
      producer.insert(QString("version"), QJsonValue(VERSION));
      producer.insert(QString("revision"), QJsonValue(revision));

      // sorted iterator
      QMapIterator<QString, QString> i(score->metaTags());
      while (i.hasNext()) {
            i.next();
            if (i.value().isEmpty())
                  continue;
            scoremeta.insert(i.key(), QJsonValue(i.value()));
            }
      scoreobj.insert(QString("file"), score->fileInfo()->fileName());
      scoreobj.insert(QString("meta"), scoremeta);

      toplev.insert(QString("producer"), producer);
      toplev.insert(QString("score"), scoreobj);
      toplev.insert(QString("soundfonts"), _soundfonts);
      _attr.setObject(toplev);
      }

//---------------------------------------------------------
//   hasSoundfont
//---------------------------------------------------------

bool AudioAttribution::hasSoundfont(const QString& file)
      {
      QString tfile = file.trimmed();
      return _soundfonts.contains(tfile);
      }

//---------------------------------------------------------
//   registerSoundfont
//---------------------------------------------------------

void AudioAttribution::registerSoundfont(const QString& file, const QMap<QString, QString>& metadata)
      {
      QString tfile = file.trimmed();
      if (_soundfonts.contains(tfile)) {
            fprintf(stderr, "skipping duplicate soundfont attribution for <%s>\n", qPrintable(tfile));
            return;
            }
      fprintf(stderr, "registering soundfont <%s>\n", qPrintable(tfile));
      QJsonObject sf;

      QMapIterator<QString, QString> i(metadata);
      while (i.hasNext()) {
            i.next();
            if (sf.contains(i.key()))
                  continue;
            QString val = i.value().trimmed();
            if (val.isEmpty())
                  continue;
            sf.insert(i.key(), QJsonValue(val));
            }

      _soundfonts.insert(tfile, sf);
      }

//---------------------------------------------------------
//   getAsID3
//---------------------------------------------------------

QByteArray AudioAttribution::getAsID3()
      {
      QByteArray attribution = this->getAttribution();
      int frameSize = /* encoding */ 1 + attribution.size() + /* NUL */ 1;
      // NUL-pad up to multiples of 16 to be nice
      int totalSize = (10 + 10 + frameSize + 15) % 16;
      int tagSize = totalSize - 10;
      QByteArray tag(totalSize, '\0');

      // ID3v2.4 tag header
      tag.insert(0, "ID3\x04", 4);
      tag.insert(6, ((unsigned int)tagSize >> 21) & 0x7F);
      tag.insert(7, ((unsigned int)tagSize >> 14) & 0x7F);
      tag.insert(8, ((unsigned int)tagSize >> 7) & 0x7F);
      tag.insert(9, (unsigned int)tagSize & 0x7F);

      // frame header
      tag.insert(10, "TENC", 4);
      tag.insert(14, ((unsigned int)frameSize >> 21) & 0x7F);
      tag.insert(15, ((unsigned int)frameSize >> 14) & 0x7F);
      tag.insert(16, ((unsigned int)frameSize >> 7) & 0x7F);
      tag.insert(17, (unsigned int)frameSize & 0x7F);
      // frame body
      tag.insert(20, /* UTF-8 */ '\x03');
      tag.insert(21, attribution);

      return tag;
      }

}
