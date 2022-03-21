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

#include <memory>

#include "StreamFormatter.h"


StreamFormatter::StreamFormatter(Stream *stream)
    : m_stream(stream)
{}

void StreamFormatter::send_raw(const void *buf,
                               std::size_t len,
                               const std::type_info &type)
{
    std::size_t buf_len = len;
    uint8_t *ptr = (uint8_t *)buf;

    log_stream(ptr, buf_len, type, 's');

    m_stream->send(buf, len);
}

void StreamFormatter::recv_raw(void *buf,
                               std::size_t len,
                               const std::type_info &type)
{
    std::size_t buf_len = len;
    uint8_t *ptr = (uint8_t *)buf;

    if (bufferSize > 0) {
        std::size_t buffered = std::min(len, bufferSize);
        memcpy(buf, buffer, buffered);
        bufferSize -= buffered;
        len -= buffered;
        buf = (uint8_t *)buf + buffered;
    }

    if (len > 0) {
        m_stream->recv(buf, len);
    }

    log_stream(ptr, buf_len, type, 'r');
}

std::string StreamFormatter::recv_string(std::size_t length)
{
    std::unique_ptr<char[]> buffer(new char[length]);
    recv_raw(buffer.get(), length, typeid(char[]));
    return std::string(buffer.get(), buffer.get() + length);
}

void StreamFormatter::forward_raw(Stream &output, void *buf, std::size_t len)
{
    //if (bufferSize > 0) {
        recv_raw(buf, len, typeid(buf));
        output.send(buf, len);
    //} else {
    //    m_stream->forward(output, buf, len);
    //}
}

void StreamFormatter::forward_directly(Stream &output, std::size_t len)
{
    /*
    if (bufferSize > 0) {
        std::size_t buffered = std::min(len, bufferSize);
        output.send(buffer, std::min(len, bufferSize));
        bufferSize -= buffered;
        len -= buffered;
    }

    if (len > 0) {
        m_stream->forward_directly(output, len);
    }
    */

    char buf[len];
    forward_raw(output, (void *)buf, len);
}

void StreamFormatter::log_stream(const uint8_t *buf,
                                 std::size_t len,
                                 const std::type_info &type,
                                 const char dir)
{
        static std::unordered_set<std::size_t> types;
        const char *typeName = type.name();
        std::size_t typeHash = type.hash_code();
        uint8_t *typeHashPtr = (uint8_t *)&typeHash;
        const char *lenPtr = static_cast<char*>(static_cast<void*>(&len)); // TAKE A LOOK AT THIS

        // Log info of packet
        Log::packets() << std::hex << std::setw(2) << std::setfill('0')
        << m_stream->side();
        Log::packets().put(':');

        Log::packets() << std::hex << std::setw(2) << std::setfill('0')
        << m_stream->clientServerId();
        Log::packets().put(':');

        Log::packets() << std::hex << std::setw(2) << std::setfill('0')
        << (int)dir;
        Log::packets().put(':');

        for (int i = 0; i < sizeof(typeHash); ++i)
        {
            Log::packets() << std::hex << std::setw(2) << std::setfill('0')
            << (int)typeHashPtr[i];
        }
        Log::packets().put(':');

        for (int i = 0; i < sizeof(len); ++i)
        {
            Log::packets() << std::hex << std::setw(2) << std::setfill('0')
            << (int)lenPtr[i];
        }
        Log::packets().put(':');

        for (int i = 0; i < len; ++i)
        {
            Log::packets() << std::hex << std::setw(2) << std::setfill('0')
            << (int)buf[i];
        }
        Log::packets().put(':');

        // Log info of packet type
        if (types.find(typeHash) == types.end())
        {
            types.insert(typeHash);
            for (int i = 0; i < strlen(typeName); ++i)
            {
                Log::types() << std::hex << std::setw(2) << std::setfill('0')
                << (int)typeName[i];
            }
            Log::types().put(':');

            for (int i = 0; i < sizeof(std::size_t); ++i)
            {
                Log::types() << std::hex << std::setw(2) << std::setfill('0')
                << (int)typeHashPtr[i];
            }
            Log::types().put(':');
        }
}
