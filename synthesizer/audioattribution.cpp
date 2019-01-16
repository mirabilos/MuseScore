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
      int totalSize = (10 + 10 + frameSize + 15) & ~15;
      int tagSize = totalSize - 10;
      QByteArray tag(totalSize, '\0');

      // ID3v2.4 tag header
      tag.replace(0, 4, "ID3\x04", 4);
      tag[6] = ((unsigned int)tagSize >> 21) & 0x7F;
      tag[7] = ((unsigned int)tagSize >> 14) & 0x7F;
      tag[8] = ((unsigned int)tagSize >> 7) & 0x7F;
      tag[9] = (unsigned int)tagSize & 0x7F;

      // frame header
      tag.replace(10, 4, "TENC", 4);
      tag[14] = ((unsigned int)frameSize >> 21) & 0x7F;
      tag[15] = ((unsigned int)frameSize >> 14) & 0x7F;
      tag[16] = ((unsigned int)frameSize >> 7) & 0x7F;
      tag[17] = (unsigned int)frameSize & 0x7F;
      // frame body
      tag[20] = /* UTF-8 */ '\x03';
      tag.replace(21, attribution.size(), attribution);

      // consistency check
      if (tag.size() != totalSize)
            fprintf(stderr, "AudioAttribution::getAsID3: internal error: size (%d, %d) mismatch!\n", tag.size(), totalSize);
      return tag;
      }

}
