// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <vector>

#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/intents/web_intent_picker.h"
#include "chrome/browser/ui/intents/web_intent_picker_delegate.h"
#include "chrome/browser/ui/tab_contents/tab_contents_wrapper.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/location_bar/location_icon_view.h"
#include "chrome/browser/ui/views/toolbar_view.h"
#include "chrome/browser/ui/views/window.h"
#include "grit/generated_resources.h"
#include "grit/theme_resources.h"
#include "grit/theme_resources_standard.h"
#include "grit/ui_resources.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/canvas_skia.h"
#include "ui/gfx/image/image.h"
#include "ui/views/bubble/bubble_delegate.h"
#include "ui/views/controls/button/image_button.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/box_layout.h"

namespace {

// The space in pixels between the top-level groups in the dialog.
const int kContentAreaSpacing = 18;

// The space in pixels between the top-level groups and the dialog border.
const int kContentAreaBorder = 12;

// The space in pixels between controls in a group.
const int kControlSpacing = 6;

// The size, relative to default, of the Chrome Web store label.
const int kWebStoreLabelFontDelta = -2;

// Create a new image where |image| is drawn on top of the image identified by
// |bg_id|. The result is drawn to |out_image|.
void CreateButtonImage(int bg_id, const SkBitmap& image, SkBitmap* out_image) {
  ResourceBundle& rb = ResourceBundle::GetSharedInstance();
  SkBitmap bg_bitmap = rb.GetImageNamed(bg_id);
  bg_bitmap.copyTo(out_image, SkBitmap::kARGB_8888_Config);

  SkPaint paint;
  paint.setXfermode(SkXfermode::Create(SkXfermode::kSrcOver_Mode));

  SkCanvas bg_canvas(*out_image);
  bg_canvas.drawBitmap(image,
                       SkIntToScalar((bg_bitmap.width() - image.width()) / 2),
                       SkIntToScalar((bg_bitmap.height() - image.height()) / 2),
                       &paint);
}

// Set the images on |button| for states normal, hot, and pushed.
// The images are generated by drawing |image| on top of the identified by
// |bg_normal_id|, |bg_hot_id|, and |bg_pushed_id| respectively.
void SetButtonImages(views::ImageButton* button,
                     const SkBitmap& image,
                     int bg_normal_id, int bg_hot_id, int bg_pushed_id) {
  SkBitmap image_normal;
  CreateButtonImage(bg_normal_id, image, &image_normal);
  button->SetImage(views::CustomButton::BS_NORMAL, &image_normal);

  SkBitmap image_hot;
  CreateButtonImage(bg_hot_id, image, &image_hot);
  button->SetImage(views::CustomButton::BS_HOT, &image_hot);

  SkBitmap image_pushed;
  CreateButtonImage(bg_pushed_id, image, &image_pushed);
  button->SetImage(views::CustomButton::BS_PUSHED, &image_pushed);
}

}  // namespace

// Views implementation of WebIntentPicker.
class WebIntentPickerView : public views::BubbleDelegateView,
                            public views::ButtonListener,
                            public WebIntentPicker {
 public:
  WebIntentPickerView(views::View* anchor_view,
                      TabContentsWrapper* tab_contents,
                      WebIntentPickerDelegate* delegate);
  virtual ~WebIntentPickerView();

  // views::ButtonListener implementation.
  virtual void ButtonPressed(views::Button* sender,
                             const views::Event& event) OVERRIDE;

  // views::WidgetDelegate implementation.
  virtual void WindowClosing() OVERRIDE;

  // WebIntentPicker implementation.
  virtual void SetServiceURLs(const std::vector<GURL>& urls) OVERRIDE;
  virtual void SetServiceIcon(size_t index, const SkBitmap& icon) OVERRIDE;
  virtual void SetDefaultServiceIcon(size_t index) OVERRIDE;
  virtual void Close() OVERRIDE;
  virtual TabContents* SetInlineDisposition(const GURL& url) OVERRIDE;

 protected:
  // views::BubbleDelegateView overrides:
  void Init() OVERRIDE;

 private:
  // A weak pointer to the WebIntentPickerDelegate to notify when the user
  // chooses a service or cancels.
  WebIntentPickerDelegate* delegate_;

  // A weak pointer to the hbox that contains the buttons used to choose the
  // service.
  views::View* button_hbox_;

  // A vector of weak pointers to each of the service buttons.
  std::vector<views::Button*> buttons_;

  // A weak pointer to the plus button, used to search for more services on the
  // Chrome Web Store.
  views::ImageButton* plus_button_;

  DISALLOW_COPY_AND_ASSIGN(WebIntentPickerView);
};

// static
WebIntentPicker* WebIntentPicker::Create(Browser* browser,
                                         TabContentsWrapper* wrapper,
                                         WebIntentPickerDelegate* delegate) {
  // Find where to point the bubble at.
  BrowserView* browser_view =
      BrowserView::GetBrowserViewForBrowser(browser);
  views::View* anchor_view =
      browser_view->toolbar()->location_bar()->location_icon_view();
  WebIntentPickerView* bubble_delegate =
      new WebIntentPickerView(anchor_view, wrapper, delegate);
  browser::CreateViewsBubble(bubble_delegate);
  bubble_delegate->Show();
  return bubble_delegate;
}

WebIntentPickerView::WebIntentPickerView(views::View* anchor_view,
                                         TabContentsWrapper* wrapper,
                                         WebIntentPickerDelegate* delegate)
    : BubbleDelegateView(anchor_view, views::BubbleBorder::TOP_LEFT),
      delegate_(delegate),
      button_hbox_(NULL),
      plus_button_(NULL) {
}

WebIntentPickerView::~WebIntentPickerView() {
}

void WebIntentPickerView::ButtonPressed(views::Button* sender,
                                        const views::Event& event) {
  // TODO(binji) When we support the plus button, pressing it should forward the
  // user to services in the Chrome Web Store that provide this intent.
  if (sender == plus_button_)
    return;

  std::vector<views::Button*>::iterator iter =
      std::find(buttons_.begin(), buttons_.end(), sender);
  DCHECK(iter != buttons_.end());

  size_t index = iter - buttons_.begin();
  delegate_->OnServiceChosen(index);
}

void WebIntentPickerView::WindowClosing() {
  delegate_->OnCancelled();
}

void WebIntentPickerView::SetServiceURLs(const std::vector<GURL>& urls) {
  for (size_t i = 0; i < urls.size(); ++i) {
    views::ImageButton* button = new views::ImageButton(this);
    button->SetTooltipText(UTF8ToUTF16(urls[i].spec().c_str()));
    button_hbox_->AddChildView(button);
    buttons_.push_back(button);
  }

  // Add the '+' button, to use the Chrome Web Store.
  ResourceBundle& rb = ResourceBundle::GetSharedInstance();
  plus_button_ = new views::ImageButton(this);
  plus_button_->SetTooltipText(
      l10n_util::GetStringUTF16(IDS_FIND_MORE_INTENT_HANDLER_TOOLTIP));
  SetButtonImages(plus_button_, rb.GetImageNamed(IDR_SIDETABS_NEW_TAB),
      IDR_BROWSER_ACTION, IDR_BROWSER_ACTION_H, IDR_BROWSER_ACTION_P);
  button_hbox_->AddChildView(plus_button_);
}

void WebIntentPickerView::SetServiceIcon(size_t index, const SkBitmap& icon) {
  views::ImageButton* button =
      static_cast<views::ImageButton*>(button_hbox_->child_at(index));
  SetButtonImages(button, icon,
      IDR_BROWSER_ACTION, IDR_BROWSER_ACTION_H, IDR_BROWSER_ACTION_P);

  Layout();
  SizeToContents();
}

void WebIntentPickerView::SetDefaultServiceIcon(size_t index) {
  ResourceBundle& rb = ResourceBundle::GetSharedInstance();
  SetServiceIcon(index, rb.GetImageNamed(IDR_DEFAULT_FAVICON));
}

void WebIntentPickerView::Close() {
  GetWidget()->Close();
}

TabContents* WebIntentPickerView::SetInlineDisposition(const GURL& url) {
  // TODO(gbillock): add support here.
  return NULL;
}

void WebIntentPickerView::Init() {
  SetLayoutManager(new views::BoxLayout(
      views::BoxLayout::kHorizontal,
      kContentAreaBorder,  // inside border horizontal spacing
      kContentAreaBorder,  // inside border vertical spacing
      kContentAreaSpacing));  // between child spacing

  ResourceBundle& rb = ResourceBundle::GetSharedInstance();
  SkBitmap image_bitmap = rb.GetImageNamed(IDR_PAGEINFO_INFO);
  views::ImageView* image = new views::ImageView();
  image->SetImage(image_bitmap);
  image->SetVerticalAlignment(views::ImageView::LEADING);
  AddChildView(image);

  views::View* main_content = new views::View();
  main_content->SetLayoutManager(new views::BoxLayout(
      views::BoxLayout::kVertical,
      0,  // inside border horizontal spacing
      0,  // inside border vertical spacing
      kContentAreaSpacing));  // between child spacing

  views::Label* top_label = new views::Label(
      l10n_util::GetStringUTF16(IDS_CHOOSE_INTENT_HANDLER_MESSAGE));
  top_label->SetHorizontalAlignment(views::Label::ALIGN_LEFT);
  main_content->AddChildView(top_label);

  button_hbox_ = new views::View();
  button_hbox_->SetLayoutManager(new views::BoxLayout(
      views::BoxLayout::kHorizontal,
      0,  // inside border horizontal spacing
      0,  // inside border vertical spacing
      kControlSpacing));  // between child spacing
  main_content->AddChildView(button_hbox_);

  views::Label* bottom_label = new views::Label(
      l10n_util::GetStringUTF16(IDS_FIND_MORE_INTENT_HANDLER_MESSAGE));
  bottom_label->SetMultiLine(true);
  bottom_label->SetHorizontalAlignment(views::Label::ALIGN_LEFT);
  bottom_label->SetFont(
      bottom_label->font().DeriveFont(kWebStoreLabelFontDelta));
  main_content->AddChildView(bottom_label);

  AddChildView(main_content);
}
