//==============================================================================
//
//  Multiplex
//
//  Created by Getroot
//  Copyright (c) 2023 AirenSoft. All rights reserved.
//
//==============================================================================
#pragma once

#include <base/ovlibrary/ovlibrary.h>
#include <base/common_types.h>
#include <pugixml-1.9/src/pugixml.hpp>

#include <base/info/playlist.h>
#include <mediarouter/mediarouter_stream_tap.h>

/*
<?xml version="1.0" encoding="UTF-8"?>
<Multiplex>
    
    <OutputStream>
        <Name>tv1</Name>
    </OutputStream>

    <Playlists>
        <Playlist>
            <Name>LLHLS ABR</Name>
            <FileName>abr</FileName> <!-- FileName must be unique in the application -->
            
            <Rendition>
                <Name>1080p</Name>
                <Video>tv1_video</Video>
                <Audio>tv1_audio</Audio>
            </Rendition>
            <Rendition>
                <Name>720p</Name>
                <Video>tv3_video</Video>
                <Audio>tv3_audio</Audio>
            </Rendition>
        </Playlist>
    </Playlists>

    <SourceStreams>
        <SourceStream>
            <Name>tv1</Name>
            <Url>stream://default/live/tv1</Url>
            <TrackMap>
                <Track>
                    <SourceTrackName>bypass_video</SourceTrackName>
                    <NewTrackName>tv1_video</NewTrackName>
                </Track>
                <Track>
                    <SourceTrackName>bypass_audio</SourceTrackName>
                    <NewTrackName>tv1_audio</NewTrackName>
                </Track>
                <Track>
                    <SourceTrackName>opus</SourceTrackName>
                    <NewTrackName>tv1_opus</NewTrackName>
                </Track>
            </TrackMap>
        </SourceStream>
        <SourceStream>
            <Name>tv2</Name>
            <Url>stream://default/live/tv2</Url>

            <TrackMap>
                <Track>
                    <SourceTrackName>bypass_video</SourceTrackName>
                    <NewTrackName>tv2_video</NewTrackName>
                </Track>
                <Track>
                    <SourceTrackName>bypass_audio</SourceTrackName>
                    <NewTrackName>tv2_audio</NewTrackName>
                </Track>
                <Track>
                    <SourceTrackName>opus</SourceTrackName>
                    <NewTrackName>tv2_opus</NewTrackName>
                </Track>
            </TrackMap>

        </SourceStream>
        <SourceStream>
            <Name>tv3</Name>
            <Url>stream://default/live/tv3</Url>

            <TrackMap>
                <Track>
                    <SourceTrackName>bypass_video</SourceTrackName>
                    <NewTrackName>tv3_video</NewTrackName>
                </Track>
                <Track>
                    <SourceTrackName>bypass_audio</SourceTrackName>
                    <NewTrackName>tv3_audio</NewTrackName>
                </Track>
                <Track>
                    <SourceTrackName>opus</SourceTrackName>
                    <NewTrackName>tv3_opus</NewTrackName>
                </Track>
            </TrackMap>

        </SourceStream>
    </SourceStreams>
    
</Multiplex>
*/

namespace pvd
{
    constexpr const char* MultiplexFileExtension = "mux";
    
    class MultiplexProfile
    {
    public:

        class SourceStream
        {
        public:
            ov::String GetName() const
            {
                return _name;
            }

            ov::String GetUrlStr() const
            {
                return _url_str;
            }

            std::shared_ptr<ov::Url> GetUrl() const
            {
                return _url;
            }

            std::map<ov::String, ov::String> GetTrackMap() const
            {
                return _track_map;
            }

            bool GetNewTrackName(const ov::String &source_track_name, ov::String &new_track_name) const
            {
                auto it = _track_map.find(source_track_name);
                if (it == _track_map.end())
                {
                    return false;
                }

                new_track_name = it->second;
                return true;
            }

            std::shared_ptr<MediaRouterStreamTap> GetStreamTap()
            {
                if (_stream_tap == nullptr)
                {
                    _stream_tap = MediaRouterStreamTap::Create();
                }

                return _stream_tap;
            }

            // Setter
            void SetName(const ov::String &name)
            {
                _name = name;
            }

            bool SetUrl(const ov::String &url)
            {
                _url_str = url;
                _url = ov::Url::Parse(url);
                if (_url == nullptr)
                {
                    return false;
                }

                return true;
            }

            void AddTrackMap(const ov::String &source_track_name, const ov::String &new_track_name)
            {
                _track_map.emplace(source_track_name, new_track_name);
            }

            // equal operator
            bool operator==(const SourceStream &other) const
            {
                if (_name != other._name)
                {
                    return false;
                }

                if (_url_str != other._url_str)
                {
                    return false;
                }

                if (_track_map.size() != other._track_map.size())
                {
                    return false;
                }

                for (auto &track_map : _track_map)
                {
                    auto it = other._track_map.find(track_map.first);
                    if (it == other._track_map.end())
                    {
                        return false;
                    }

                    if (it->second != track_map.second)
                    {
                        return false;
                    }
                }

                return true;
            }

            // not equals operator
            bool operator!=(const SourceStream &other) const
            {
                return !(*this == other);
            }

        private:
            ov::String _name;
            ov::String _url_str;
            std::shared_ptr<ov::Url> _url;

            std::shared_ptr<MediaRouterStreamTap> _stream_tap = nullptr;

            // source track name : new track name
            std::map<ov::String, ov::String> _track_map;
        };

        static std::tuple<std::shared_ptr<MultiplexProfile>, ov::String> CreateFromXMLFile(const ov::String &file_path);
        static std::tuple<std::shared_ptr<MultiplexProfile>, ov::String> CreateFromJsonObject(const Json::Value &object);

        MultiplexProfile() = default;
        ~MultiplexProfile() = default;

        bool LoadFromXMLFile(const ov::String &file_path);
        bool LoadFromJsonObject(const Json::Value &object);

        std::shared_ptr<MultiplexProfile> Clone() const;

        ov::String GetLastError() const;
        
        std::chrono::system_clock::time_point GetCreatedTime() const;
        ov::String GetOutputStreamName() const;
        const std::vector<std::shared_ptr<info::Playlist>> &GetPlaylists() const;
        const std::vector<std::shared_ptr<SourceStream>> &GetSourceStreams() const;

        CommonErrorCode SaveToXMLFile(const ov::String &file_path) const;
        CommonErrorCode ToJsonObject(Json::Value &root_object) const;

        // equal operator
        bool operator==(const MultiplexProfile &other) const;

    private:
        bool ReadOutputStreamNode(const pugi::xml_node &root_node);
        bool ReadPlaylistsNode(const pugi::xml_node &root_node);
        bool ReadSourceStreamsNode(const pugi::xml_node &root_node);
        
        std::chrono::system_clock::time_point _created_time;
        
        ov::String _output_stream_name;
        std::vector<std::shared_ptr<info::Playlist>> _playlists;
        std::vector<std::shared_ptr<SourceStream>> _source_streams;

        // For validation of new track names and playlist names
        std::map<ov::String, bool> _new_track_names;

        mutable ov::String _last_error;
    };
}