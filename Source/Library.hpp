#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <set>
#include <sstream>
#include <string_view>
#include <vector>

#include <qmainwindow.h>
#include <QAbstractTableModel>

#include <QTableView>

#include "MusicPlayer.hpp"

namespace fs = std::experimental::filesystem;

class Album;
class Artist;

class Track
{
public:
  template <typename tType>
  Track(std::string &&aLibraryLine)
    : mMedia(nullptr)
    , mParsed(false)
  {
  }

  Track(Track const &aTrack) = delete;

  Track()
    : mTrackNumber(0)
    , mMedia(nullptr)
    , mParsed(false)
  {
    mTrackNumber = 0;
    mMedia = nullptr;
  }

  // Rows that actually display.
  std::string_view mTitle;
  std::string_view mArtistName;
  std::string_view mAlbumName;
  int64_t mTrackNumber;

  // Utility
  std::string_view mLocation;
  libvlc_media_t *mMedia;
  bool mParsed;

  // Future
  Artist *mArtist;
  Album *mAlbum;
};

class Playlist
{
public:
  template <typename tType>
  Playlist(tType &aName)
    : mName(aName)
  {
  }

  std::string mName;
  std::vector<Track*> mTracks;
};

class Album
{
public:
  template <typename tType>
  Album(tType &aName)
    : mName(aName)
  {
  }

  std::string mName;
  std::string mArt;
  std::vector<Track*> mTracks;
};

class Artist
{
public:
  template <typename tType>
  Artist(tType &aName)
    : mName(aName)
  {
  }

  std::string mName;
  std::string mArt;
  std::vector<Album*> mAlbums;
};

struct Library : public QAbstractTableModel
{
  Library(QMainWindow *aMainWindow, QTableView *mView, MusicPlayer *mMusicPlayer);
  Library(Library const &aTrack) = delete;

  // Get* will create an Artist if artist doesn't exist.
  Artist* GetArtist(std::string_view aArtist);
  Album* GetAlbum(std::string_view aAlbum);
  Playlist* GetPlaylist(std::string_view aPlaylist);

  // Find* will return nullptr if not found.
  Artist* FindArtist(std::string_view aArtist);
  Album* FindAlbum(std::string_view aAlbum);
  Playlist* FindPlaylist(std::string_view aPlaylist);

  void LoadLibrary(std::string aPath);
  void ScanLibrary(fs::path aPath);
  //void ParseFiles(std::vector<fs::path> &aFiles);
  void ParseAlbum(std::string_view aAlbum, std::string_view aArtist, Track *aTrack);
  void ParseArtist(std::string_view aArtist);


  void PlayTrack(const QModelIndex &mIndex);



  int rowCount(const QModelIndex &) const
  {
    return static_cast<int>(mTracks.size());
  }

  int columnCount(const QModelIndex &) const
  {
    return 5;
  }

  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
  bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
  bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

  // File Location to Track
  std::vector<std::unique_ptr<Track>> mTracks;

  std::unordered_map<std::string, std::unique_ptr<Playlist>> mPlaylists;
  std::unordered_map<std::string, std::unique_ptr<Artist>> mArtists;
  std::unordered_map<std::string, std::unique_ptr<Album>> mAlbums;

  mutable int mThrash = 0;

  QMainWindow *mMainWindow;
  libvlc_instance_t *mInstance;
  QTableView *mView;
  MusicPlayer *mMusicPlayer;
  //QSqlTableModel mModel;
};