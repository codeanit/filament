/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GLTFIO_ASSETLOADER_H
#define GLTFIO_ASSETLOADER_H

#include <utils/Entity.h>

namespace filament {
    class Camera;
    class Engine;
    class IndexBuffer;
    class Material;
    class MaterialInstance;
    class Scene;
    class Texture;
    class VertexBuffer;
}

namespace gltfio {

class FilamentAsset;

/**
 * Consumes glTF 2.0 content (either JSON or GLB) and produces bundles of Filament renderables,
 * material instances, vertex buffers, index buffers, textures, and light sources.
 *
 * For JSON-based assets, this does not load external buffer data or image data. Clients must
 * manually query the URI's in the returned FilamentAsset object and load external data manually.
 *
 * This class also owns a cache of Material objects that may be re-used across multiple loads.
 */
class AssetLoader {
public:

    /**
     * Takes a weak reference to an Engine, used only for the creation of various Filament objects.
     */
    AssetLoader(filament::Engine* engine);

    /**
     * When the loader is destroyed, it does not automatically free the cache of materials; see
     * destroyMaterials().
     */
    ~AssetLoader();

    FilamentAsset* createAssetFromJson(uint8_t const* bytes, uint32_t nbytes);
    FilamentAsset* createAssetFromBinary(uint8_t const* bytes, uint32_t nbytes);
    void destroyAsset(FilamentAsset* asset);

    size_t getMaterialsCount() const noexcept;
    size_t getMaterials(filament::Material* mats, size_t count) const noexcept;
    void destroyMaterials();
};

struct BufferAccessor;
struct PixelAccessor;

/**
 * Owns a bundle of Filament objects that have been created by gltfio::AssetLoader.
 *
 * Holds strong references to entities (renderables and lights) that have been loaded from a glTF
 * asset, as well as strong references to Texture, VertexBuffer, IndexBuffer, and MaterialInstance.
 *
 * Holds a weak instance to filament::Engine to allow destruction of Filament objects.
 *
 * Clients must iterate over texture uri's and call Texture::setImage(), unless the asset was
 * loaded from a GLB file.
 *
 * Similarly, clients must iterate over buffer uri's and call VertexBuffer::setBufferAt()
 * and filament::IndexBuffer::setBuffer().
 *
 * TODO: This supports skinning but not animation or morphing.
 */
class FilamentAsset {
public:
    // Obtain renderables and light sources.
    size_t getEntitiesCount() const noexcept;
    const utils::Entity* getEntities() const noexcept;

    // Obtain material instances that are already bound to renderables and textures.
    size_t getMaterialInstancesCount() const noexcept;
    filament::MaterialInstance* const* getMaterialInstances() const noexcept;

    // Obtain loading instructions for vertex buffers and index buffers.
    size_t getBufferAccessorCount() const noexcept;
    BufferAccessor const* getBufferAccessors() const noexcept;

    // Obtain loading instructions for texture images.
    size_t getPixelAccessorCount() const noexcept;
    PixelAccessor const* getPixelAccessors() const noexcept;

    // Pushes glTF camera settings (if any) to the client's camera object.
    void updateCamera(filament::Camera* camera) const noexcept;

private:
    // disallow creation on the stack
    FilamentAsset() noexcept = default;
    ~FilamentAsset() = default;
};

struct BufferAccessor {
    const char* uri;
    filament::VertexBuffer* vertexBuffer;
    filament::IndexBuffer* indexBuffer;
    int bufferIndex;
    uint32_t byteOffset;
    uint32_t byteSize;
};

struct PixelAccessor {
    const char* uri;
    filament::Texture* texture;
    size_t level;
    uint32_t xoffset;
    uint32_t yoffset;
    uint32_t width;
    uint32_t height;
};

} // namespace gltfio

#endif // GLTFIO_ASSETLOADER_H
