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

#ifndef __AUDIOATTRIBUTION_H__
#define __AUDIOATTRIBUTION_H__

namespace Ms {

//---------------------------------------------------------
//   AudioAttribution
//---------------------------------------------------------

class AudioAttribution {
      QJsonDocument _attr;
      QJsonObject _soundfonts;

   public:
      AudioAttribution(Score* score);
      bool hasSoundfont(const QString& file);
      void registerSoundfont(const QString& file, const QMap<QString, QString>& metadata);
      QByteArray getAttribution() const { return _attr.toJson(QJsonDocument::Compact); }
      QByteArray getAsID3();
      };

} // namespace Ms
#endif
