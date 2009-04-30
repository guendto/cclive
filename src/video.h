/*
 * cclive Copyright (C) 2009 Toni Gundogdu. This file is part of cclive.
 * 
 * cclive is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * cclive is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef video_h
#define video_h

class RuntimeException;

class VideoProperties {
public:
    VideoProperties();
public:
    void setId          (const std::string&);
    void setLink        (std::string);
    void setHost        (const std::string&);
    void setDomain      (const std::string&);
    void setTitle       (const std::string&);
    void setLength      (const double);
    void setContentType (const std::string&);
    void setInitial     (const double);
public:
    const std::string&  getId           () const;
    const std::string&  getLink         () const;
    const std::string&  getHost         () const;
    const std::string&  getDomain       () const;
    const double        getLength       () const;
    const double        getInitial      () const;
    const std::string&  getContentType  () const;
    const std::string&  getSuffix       () const;
    const std::string&  getFilename     () const;
private:
    void formatOutputFilename           ();
    void defaultOutputFilenameFormatter (std::stringstream&);
    void customOutputFilenameFormatter  (std::stringstream&);
private:
    std::string id;
    std::string link;
    std::string host;
    std::string domain;
    std::string title;
    double      length;
    double      initial;
    std::string contentType;
    std::string suffix;
    std::string filename;
public:
    class NothingToDoException : public RuntimeException {
    public:
        NothingToDoException();
    };
};

#endif
