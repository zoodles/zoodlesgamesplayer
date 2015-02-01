/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef GFX_CLIENTTILEDPAINTEDLAYER_H
#define GFX_CLIENTTILEDPAINTEDLAYER_H

#include "ClientLayerManager.h"         // for ClientLayer, etc
#include "Layers.h"                     // for PaintedLayer, etc
#include "mozilla/RefPtr.h"             // for RefPtr
#include "mozilla/layers/TiledContentClient.h"
#include "nsDebug.h"                    // for NS_RUNTIMEABORT
#include "nsRegion.h"                   // for nsIntRegion

class gfxContext;

namespace mozilla {
namespace layers {

class ShadowableLayer;
class SpecificLayerAttributes;

/**
 * An implementation of PaintedLayer that ONLY supports remote
 * composition that is backed by tiles. This painted layer implementation
 * is better suited to mobile hardware to work around slow implementation
 * of glTexImage2D (for OGL compositors), and restrait memory bandwidth.
 *
 * Tiled PaintedLayers use a different protocol compared with other
 * layers. A copy of the tiled buffer is made and sent to the compositing
 * thread via the layers protocol. Tiles are uploaded by the buffers
 * asynchonously without using IPC, that means they are not safe for cross-
 * process use (bug 747811). Each tile has a TextureHost/Client pair but
 * they communicate directly rather than using the Texture protocol.
 *
 * There is no ContentClient for tiled layers. There is a ContentHost, however.
 */
class ClientTiledPaintedLayer : public PaintedLayer,
                               public ClientLayer
{
  typedef PaintedLayer Base;

public:
  explicit ClientTiledPaintedLayer(ClientLayerManager* const aManager,
                                  ClientLayerManager::PaintedLayerCreationHint aCreationHint = LayerManager::NONE);

protected:
  ~ClientTiledPaintedLayer();

  virtual void PrintInfo(std::stringstream& aStream, const char* aPrefix) MOZ_OVERRIDE;

public:
  // Override name to distinguish it from ClientPaintedLayer in layer dumps
  virtual const char* Name() const MOZ_OVERRIDE { return "TiledPaintedLayer"; }

  // PaintedLayer
  virtual Layer* AsLayer() MOZ_OVERRIDE { return this; }
  virtual void InvalidateRegion(const nsIntRegion& aRegion) MOZ_OVERRIDE {
    mInvalidRegion.Or(mInvalidRegion, aRegion);
    mInvalidRegion.SimplifyOutward(20);
    mValidRegion.Sub(mValidRegion, mInvalidRegion);
    mLowPrecisionValidRegion.Sub(mLowPrecisionValidRegion, mInvalidRegion);
  }

  // Shadow methods
  virtual void FillSpecificAttributes(SpecificLayerAttributes& aAttrs) MOZ_OVERRIDE;
  virtual ShadowableLayer* AsShadowableLayer() MOZ_OVERRIDE { return this; }

  virtual void Disconnect() MOZ_OVERRIDE
  {
    ClientLayer::Disconnect();
  }

  virtual void RenderLayer() MOZ_OVERRIDE;

  virtual void ClearCachedResources() MOZ_OVERRIDE;

  /**
   * Helper method to find the nearest ancestor layers which
   * scroll and have a displayport. The parameters are out-params
   * which hold the return values; the values passed in may be null.
   */
  void GetAncestorLayers(LayerMetricsWrapper* aOutScrollAncestor,
                         LayerMetricsWrapper* aOutDisplayPortAncestor);

private:
  ClientLayerManager* ClientManager()
  {
    return static_cast<ClientLayerManager*>(mManager);
  }

  /**
   * For the initial PaintThebes of a transaction, calculates all the data
   * needed for that paint and any repeated transactions.
   */
  void BeginPaint();

  /**
   * Determine if we can use a fast path to just do a single high-precision,
   * non-progressive paint.
   */
  bool UseFastPath();

  /**
   * Check if the layer is being scrolled by APZ on the compositor.
   */
  bool IsScrollingOnCompositor(const FrameMetrics& aParentMetrics);

  /**
   * Helper function to do the high-precision paint.
   * This function returns true if it updated the paint buffer.
   */
  bool RenderHighPrecision(nsIntRegion& aInvalidRegion,
                           const nsIntRegion& aVisibleRegion,
                           LayerManager::DrawPaintedLayerCallback aCallback,
                           void* aCallbackData);

  /**
   * Helper function to do the low-precision paint.
   * This function returns true if it updated the paint buffer.
   */
  bool RenderLowPrecision(nsIntRegion& aInvalidRegion,
                          const nsIntRegion& aVisibleRegion,
                          LayerManager::DrawPaintedLayerCallback aCallback,
                          void* aCallbackData);

  /**
   * This causes the paint to be marked as finished, and updates any data
   * necessary to persist until the next paint.
   */
  void EndPaint();

  RefPtr<TiledContentClient> mContentClient;
  nsIntRegion mLowPrecisionValidRegion;
  BasicTiledLayerPaintData mPaintData;
};

} // layers
} // mozilla

#endif