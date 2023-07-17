/*
 *
 * Copyright 2022 Apple Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//
// delegate loop class
//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include "app.h"
#include <AppKit/AppKit.hpp>
#include <CoreFoundation/CoreFoundation.h>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

namespace
{

//
// View Delegate
//
class MyMTKViewDelegate : public MTK::ViewDelegate
{
  public:
    MyMTKViewDelegate(MTL::Device* pDevice) : MTK::ViewDelegate() {}
    ~MyMTKViewDelegate() override = default;
    void drawInMTKView(MTK::View* pView) override { _pRenderer->draw(pView); }

    void setDelegateLoop(std::shared_ptr<DelegateLoop> dlp) { _pRenderer = dlp; }

  private:
    std::shared_ptr<DelegateLoop> _pRenderer;
};

//
// App Delegate
//
class MyAppDelegate : public NS::ApplicationDelegate
{
  public:
    ~MyAppDelegate();

    NS::Menu* createMenuBar();

    void applicationWillFinishLaunching(NS::Notification* pNotification) override;
    void applicationDidFinishLaunching(NS::Notification* pNotification) override;
    bool applicationShouldTerminateAfterLastWindowClosed(NS::Application* pSender) override;

    void setDelegateLoop(std::shared_ptr<DelegateLoop> dl) { delegateLoop = dl; }

  private:
    NS::Window*  _pWindow;
    MTK::View*   _pMtkView;
    MTL::Device* _pDevice;

    std::unique_ptr<MyMTKViewDelegate> _pViewDelegate;
    std::shared_ptr<DelegateLoop>      delegateLoop;
};

MyAppDelegate::~MyAppDelegate()
{
    _pMtkView->release();
    _pWindow->release();
    _pDevice->release();
}

NS::Menu*
MyAppDelegate::createMenuBar()
{
    using NS::StringEncoding::UTF8StringEncoding;

    NS::Menu*     pMainMenu    = NS::Menu::alloc()->init();
    NS::MenuItem* pAppMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu*     pAppMenu     = NS::Menu::alloc()->init(NS::String::string("Appname", UTF8StringEncoding));

    NS::String* appName      = NS::RunningApplication::currentApplication()->localizedName();
    NS::String* quitItemName = NS::String::string("Quit ", UTF8StringEncoding)->stringByAppendingString(appName);
    SEL         quitCb       = NS::MenuItem::registerActionCallback("appQuit",
                                                                    [](void*, SEL, const NS::Object* pSender)
                                                                    {
                                                          auto pApp = NS::Application::sharedApplication();
                                                          pApp->terminate(pSender);
                                                      });

    NS::MenuItem* pAppQuitItem = pAppMenu->addItem(quitItemName, quitCb, NS::String::string("q", UTF8StringEncoding));
    pAppQuitItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);
    pAppMenuItem->setSubmenu(pAppMenu);

    NS::MenuItem* pWindowMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu*     pWindowMenu     = NS::Menu::alloc()->init(NS::String::string("Window", UTF8StringEncoding));

    SEL           closeWindowCb    = NS::MenuItem::registerActionCallback("windowClose",
                                                                          [](void*, SEL, const NS::Object*)
                                                                          {
                                                                 auto pApp = NS::Application::sharedApplication();
                                                                 pApp->windows()->object<NS::Window>(0)->close();
                                                             });
    NS::MenuItem* pCloseWindowItem = pWindowMenu->addItem(NS::String::string("Close Window", UTF8StringEncoding), closeWindowCb,
                                                          NS::String::string("w", UTF8StringEncoding));
    pCloseWindowItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);

    pWindowMenuItem->setSubmenu(pWindowMenu);

    pMainMenu->addItem(pAppMenuItem);
    pMainMenu->addItem(pWindowMenuItem);

    pAppMenuItem->release();
    pWindowMenuItem->release();
    pAppMenu->release();
    pWindowMenu->release();

    return pMainMenu->autorelease();
}

void
MyAppDelegate::applicationWillFinishLaunching(NS::Notification* pNotification)
{
    NS::Menu*        pMenu = createMenuBar();
    NS::Application* pApp  = reinterpret_cast<NS::Application*>(pNotification->object());
    pApp->setMainMenu(pMenu);
    pApp->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
}

void
MyAppDelegate::applicationDidFinishLaunching(NS::Notification* pNotification)
{
    CGRect frame = (CGRect){{150.0, 250.0}, {delegateLoop->getScreenWidth(), delegateLoop->getScreenHeight()}};

    _pWindow = NS::Window::alloc()->init(frame, NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled, NS::BackingStoreBuffered,
                                         false);

    _pDevice = MTL::CreateSystemDefaultDevice();

    _pMtkView = MTK::View::alloc()->init(frame, _pDevice);
    _pMtkView->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    _pMtkView->setClearColor(MTL::ClearColor::Make(0.1, 0.1, 0.1, 1.0));
    _pMtkView->setDepthStencilPixelFormat(MTL::PixelFormat::PixelFormatDepth16Unorm);
    _pMtkView->setClearDepth(1.0f);

    _pViewDelegate = std::make_unique<MyMTKViewDelegate>(_pDevice);
    _pMtkView->setDelegate(_pViewDelegate.get());
    _pViewDelegate->setDelegateLoop(delegateLoop);
    delegateLoop->initialize(_pDevice);

    _pWindow->setContentView(_pMtkView);
    _pWindow->setTitle(NS::String::string(delegateLoop->getTitle(), NS::StringEncoding::UTF8StringEncoding));

    _pWindow->makeKeyAndOrderFront(nullptr);

    NS::Application* pApp = reinterpret_cast<NS::Application*>(pNotification->object());
    pApp->activateIgnoringOtherApps(true);
}

bool
MyAppDelegate::applicationShouldTerminateAfterLastWindowClosed(NS::Application* pSender)
{
    return true;
}

} // namespace

void
Launch(std::shared_ptr<DelegateLoop> dlp)
{
    auto* pAutoreleasePool = NS::AutoreleasePool::alloc()->init();

    MyAppDelegate del;
    del.setDelegateLoop(dlp);

    auto* pSharedApplication = NS::Application::sharedApplication();
    pSharedApplication->setDelegate(&del);
    pSharedApplication->run();

    pAutoreleasePool->release();
}

//
