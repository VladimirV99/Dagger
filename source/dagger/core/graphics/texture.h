#pragma once

#include "core/core.h"
#include "core/filesystem.h"
#include <glad/glad.h>
#include <memory>

class TextureSystem;

class Texture
{
    String m_Name;
    FilePath m_Path;
    UInt32 m_Width { 0 }, m_Height { 0 }, m_Channels { 0 };
    UInt32 m_TextureId { 0 };
    Float32 m_Ratio { 0 };

    friend struct TextureSystem;

public:

    inline UInt32 Width() const
    {
        return m_Width;
    }

    inline UInt32 Height() const
    {
        return m_Height;
    }

    inline UInt32 Depth() const
    {
        return m_Channels;
    }

    inline UInt32 TextureId() const
    {
        return m_TextureId;
    }

    inline Float32 Ratio() const
    {
        return m_Ratio;
    }

    inline String Name() const
    {
        return m_Name;
    }

    Texture() = default;

    Texture(String name_, const FilePath path_, UInt8* data_, UInt32 width_, UInt32 height_, UInt32 channels_)
        : m_Name{ std::move(name_) }
        , m_Path{ path_ }
        , m_Width{ width_ }
        , m_Height{ height_ }
        , m_Channels{ channels_ }
        , m_Ratio{ (Float32)height_ / (Float32)width_ }
    {
        assert(m_Ratio > 0);

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);

        glGenTextures(1, &m_TextureId);
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, channels_ == 4 ? GL_RGBA : GL_RGB, m_Width, m_Height, 0, channels_ == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data_);
        
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    Texture(const Texture& other_) = default;

    Texture& operator=(const Texture& /*unused*/)
    {
        return *this;
    }
};