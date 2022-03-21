/*
 * Copyright (c) 2016 Michal Srb <michalsrb@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "Log.h"

std::ofstream Log::packetsStream;
std::ofstream Log::typesStream;

std::ostream &Log::error()
{
    std::cerr << "<3>";
    return std::cerr;
}

std::ostream &Log::warning()
{
    std::cerr << "<4>";
    return std::cerr;
}

std::ostream &Log::notice()
{
    std::cout << "<5>";
    return std::cout;
}

std::ostream &Log::info()
{
    std::cout << "<6>";
    return std::cout;
}

std::ostream &Log::debug()
{
    std::cout << "<7>";
    return std::cout;
}

std::ofstream &Log::packets()
{
    return packetsStream;
}

std::ofstream &Log::types()
{
    return typesStream;
}

void Log::makeLogDir()
{
    std::string path = "/var/log/vncmanager";
    std::string packetsFile = path + "/vncmanager_packets.log";
    std::string typesFile = path + "/vncmanager_types.log";

    if (mkdir(path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
        < 0 && errno != EEXIST)
    {
        throw_errno(path);
    }

    packetsStream = std::ofstream(packetsFile,
                                  std::ofstream::out |
                                  std::ofstream::trunc |
                                  std::ofstream::binary);
    packetsStream << std::unitbuf;

    typesStream = std::ofstream(typesFile,
                                std::ofstream::out |
                                std::ofstream::trunc |
                                std::ofstream::binary);
    typesStream << std::unitbuf;

    chmod(packetsFile.c_str(), DEFFILEMODE);
    chmod(typesFile.c_str(), DEFFILEMODE);
}
