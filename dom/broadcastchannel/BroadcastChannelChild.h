/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_BroadcastChannelChild_h
#define mozilla_dom_BroadcastChannelChild_h

#include "mozilla/dom/PBroadcastChannelChild.h"

namespace mozilla {

namespace ipc {
class BackgroundChildImpl;
}

namespace dom {

class BroadcastChannel;

class BroadcastChannelChild MOZ_FINAL : public PBroadcastChannelChild
{
  friend class mozilla::ipc::BackgroundChildImpl;

public:
  NS_INLINE_DECL_REFCOUNTING(BroadcastChannelChild)

  void SetParent(BroadcastChannel* aBC)
  {
    mBC = aBC;
  }

  virtual bool RecvNotify(const ClonedMessageData& aData) MOZ_OVERRIDE;

  bool IsActorDestroyed() const
  {
    return mActorDestroyed;
  }

private:
  BroadcastChannelChild(const nsAString& aOrigin,
                        const nsAString& aChannel);

  ~BroadcastChannelChild();

  virtual void ActorDestroy(ActorDestroyReason aWhy) MOZ_OVERRIDE;

  // This raw pointer is actually the parent object.
  // It's set to null when the parent object is deleted.
  BroadcastChannel* mBC;

  nsString mOrigin;
  nsString mChannel;

  bool mActorDestroyed;
};

} // dom namespace
} // mozilla namespace

#endif // mozilla_dom_BroadcastChannelChild_h