# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },

  'targets': [
    {
      'target_name': 'ash',
      'type': '<(component)',
      'dependencies': [
        '../base/base.gyp:base',
        '../base/base.gyp:base_i18n',
        '../base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '../build/temp_gyp/googleurl.gyp:googleurl',
        '../net/net.gyp:net',
        '../skia/skia.gyp:skia',
        '../third_party/icu/icu.gyp:icui18n',
        '../third_party/icu/icu.gyp:icuuc',
        '../ui/aura/aura.gyp:aura',
        '../ui/base/strings/ui_strings.gyp:ui_strings',
        '../ui/gfx/compositor/compositor.gyp:compositor',
        '../ui/ui.gyp:ui',
        '../ui/ui.gyp:ui_resources',
        '../ui/ui.gyp:ui_resources_standard',
        '../ui/views/views.gyp:views',
      ],
      'defines': [
        'ASH_IMPLEMENTATION',
      ],
      'sources': [
        # All .cc, .h under ash, except unittests
        'accelerators/accelerator_controller.cc',
        'accelerators/accelerator_controller.h',
        'accelerators/accelerator_filter.cc',
        'accelerators/accelerator_filter.h',
        'app_list/app_list.cc',
        'app_list/app_list.h',
        'app_list/app_list_groups_view.cc',
        'app_list/app_list_groups_view.h',
        'app_list/app_list_item_group_model.cc',
        'app_list/app_list_item_group_model.h',
        'app_list/app_list_item_group_view.cc',
        'app_list/app_list_item_group_view.h',
        'app_list/app_list_item_model.cc',
        'app_list/app_list_item_model.h',
        'app_list/app_list_item_model_observer.h',
        'app_list/app_list_item_view.cc',
        'app_list/app_list_item_view.h',
        'app_list/app_list_item_view_listener.h',
        'app_list/app_list_model.cc',
        'app_list/app_list_model.h',
        'app_list/app_list_view.cc',
        'app_list/app_list_view.h',
        'app_list/app_list_view_delegate.h',
        'app_list/drop_shadow_label.cc',
        'app_list/drop_shadow_label.h',
        'ash_switches.cc',
        'ash_switches.h',
        'desktop_background/desktop_background_view.cc',
        'desktop_background/desktop_background_view.h',
        'drag_drop/drag_drop_controller.cc',
        'drag_drop/drag_drop_controller.h',
        'drag_drop/drag_image_view.cc',
        'drag_drop/drag_image_view.h',
        'ime/event.cc',
        'ime/event.h',
        'ime/input_method_event_filter.cc',
        'ime/input_method_event_filter.h',
        'launcher/app_launcher_button.cc',
        'launcher/app_launcher_button.h',
        'launcher/launcher.cc',
        'launcher/launcher.h',
        'launcher/launcher_model.cc',
        'launcher/launcher_model.h',
        'launcher/launcher_model_observer.h',
        'launcher/launcher_types.h',
        'launcher/launcher_view.cc',
        'launcher/launcher_view.h',
        'launcher/tabbed_launcher_button.cc',
        'launcher/tabbed_launcher_button.h',
        'launcher/view_model.cc',
        'launcher/view_model.h',
        'launcher/view_model_utils.cc',
        'launcher/view_model_utils.h',
        'shell.cc',
        'shell.h',
        'shell_delegate.h',
        'shell_factory.h',
        'shell_window_ids.h',
        'status_area/status_area_view.cc',
        'status_area/status_area_view.h',
        'tooltips/tooltip_controller.cc',
        'tooltips/tooltip_controller.h',
        'wm/activation_controller.cc',
        'wm/activation_controller.h',
        'wm/always_on_top_controller.cc',
        'wm/always_on_top_controller.h',
        'wm/compact_layout_manager.cc',
        'wm/compact_layout_manager.h',
        'wm/compact_status_area_layout_manager.cc',
        'wm/compact_status_area_layout_manager.h',
        'wm/default_container_event_filter.cc',
        'wm/default_container_event_filter.h',
        'wm/default_container_layout_manager.cc',
        'wm/default_container_layout_manager.h',
        'wm/image_grid.cc',
        'wm/image_grid.h',
        'wm/modal_container_layout_manager.cc',
        'wm/modal_container_layout_manager.h',
        'wm/modality_event_filter.cc',
        'wm/modality_event_filter.h',
        'wm/modality_event_filter_delegate.h',
        'wm/property_util.cc',
        'wm/property_util.h',
        'wm/root_window_event_filter.cc',
        'wm/root_window_event_filter.h',
        'wm/root_window_layout_manager.cc',
        'wm/root_window_layout_manager.h',
        'wm/shadow.cc',
        'wm/shadow.h',
        'wm/shadow_controller.cc',
        'wm/shadow_controller.h',
        'wm/shadow_types.cc',
        'wm/shadow_types.h',
        'wm/shelf_layout_manager.cc',
        'wm/shelf_layout_manager.h',
        'wm/show_state_controller.h',
        'wm/show_state_controller.cc',
        'wm/stacking_controller.cc',
        'wm/stacking_controller.h',
        'wm/status_area_layout_manager.cc',
        'wm/status_area_layout_manager.h',
        'wm/toplevel_frame_view.cc',
        'wm/toplevel_frame_view.h',
        'wm/toplevel_layout_manager.cc',
        'wm/toplevel_layout_manager.h',
        'wm/toplevel_window_event_filter.cc',
        'wm/toplevel_window_event_filter.h',
        'wm/window_frame.cc',
        'wm/window_frame.h',
        'wm/window_properties.cc',
        'wm/window_properties.h',
        'wm/window_util.cc',
        'wm/window_util.h',
        'wm/workspace_controller.cc',
        'wm/workspace_controller.h',
        'wm/workspace/workspace.cc',
        'wm/workspace/workspace.h',
        'wm/workspace/workspace_manager.cc',
        'wm/workspace/workspace_manager.h',
        'wm/workspace/workspace_observer.h',
      ],
    },
    {
      'target_name': 'aura_shell_unittests',
      'type': 'executable',
      'dependencies': [
        '../base/base.gyp:base',
        '../base/base.gyp:test_support_base',
        '../chrome/chrome_resources.gyp:packed_resources',
        '../build/temp_gyp/googleurl.gyp:googleurl',
        '../skia/skia.gyp:skia',
        '../testing/gtest.gyp:gtest',
        '../third_party/icu/icu.gyp:icui18n',
        '../third_party/icu/icu.gyp:icuuc',
        '../ui/aura/aura.gyp:aura',
        '../ui/aura/aura.gyp:test_support_aura',
        '../ui/gfx/compositor/compositor.gyp:compositor_test_support',
        '../ui/ui.gyp:gfx_resources',
        '../ui/ui.gyp:ui',
        '../ui/ui.gyp:ui_resources',
        '../ui/ui.gyp:ui_resources_standard',
        '../ui/views/views.gyp:views',
        'ash',
      ],
      'sources': [
        'accelerators/accelerator_controller_unittest.cc',
        'drag_drop/drag_drop_controller_unittest.cc',
        'ime/input_method_event_filter_unittest.cc',
        'launcher/launcher_model_unittest.cc',
        'launcher/launcher_unittest.cc',
        'launcher/view_model_unittest.cc',
        'launcher/view_model_utils_unittest.cc',
        'shell_unittest.cc',
        'test/ash_unittests.cc',
        'test/aura_shell_test_base.cc',
        'test/aura_shell_test_base.h',
        'test/test_activation_delegate.cc',
        'test/test_activation_delegate.h',
        'test/test_shell_delegate.cc',
        'test/test_shell_delegate.h',
        'test/test_suite.cc',
        'test/test_suite.h',
        'tooltips/tooltip_controller_unittest.cc',
        'wm/activation_controller_unittest.cc',
        'wm/default_container_layout_manager_unittest.cc',
        'wm/image_grid_unittest.cc',
        'wm/modal_container_layout_manager_unittest.cc',
        'wm/root_window_event_filter_unittest.cc',
        'wm/shadow_controller_unittest.cc',
        'wm/shelf_layout_manager_unittest.cc',
        'wm/toplevel_layout_manager_unittest.cc',
        'wm/toplevel_window_event_filter_unittest.cc',
        'wm/workspace_controller_unittest.cc',
        'wm/workspace/workspace_manager_unittest.cc',

        '<(SHARED_INTERMEDIATE_DIR)/ui/gfx/gfx_resources.rc',
        '<(SHARED_INTERMEDIATE_DIR)/ui/ui_resources/ui_resources.rc',
        '<(SHARED_INTERMEDIATE_DIR)/ui/ui_resources_standard/ui_resources_standard.rc',
      ],
      'conditions': [
        ['use_webkit_compositor==1', {
          'dependencies': [
            '../ui/gfx/compositor/compositor.gyp:compositor',
          ],
        }, { # use_webkit_compositor!=1
          'dependencies': [
            '../ui/gfx/compositor/compositor.gyp:test_compositor',
          ],
        }],
        ['use_ibus==1', {
          'dependencies': [
            '../build/linux/system.gyp:ibus',
          ],
        }],
      ],
    },
    {
      'target_name': 'ash_shell',
      'type': 'executable',
      'dependencies': [
        '../base/base.gyp:base',
        '../base/base.gyp:base_i18n',
        '../chrome/chrome_resources.gyp:packed_resources',
        '../skia/skia.gyp:skia',
        '../third_party/icu/icu.gyp:icui18n',
        '../third_party/icu/icu.gyp:icuuc',
        '../ui/aura/aura.gyp:aura',
        '../ui/gfx/compositor/compositor.gyp:compositor',
        '../ui/gfx/compositor/compositor.gyp:compositor_test_support',
        '../ui/ui.gyp:gfx_resources',
        '../ui/ui.gyp:ui',
        '../ui/ui.gyp:ui_resources',
        '../ui/ui.gyp:ui_resources_standard',
        '../ui/views/views.gyp:views',
        '../ui/views/views.gyp:views_examples_lib',
        'ash',
      ],
      'sources': [
        'shell/app_list.cc',
        'shell/bubble.cc',
        'shell/example_factory.h',
        'shell/lock_view.cc',
        'shell/shell_main.cc',
        'shell/toplevel_window.cc',
        'shell/toplevel_window.h',
        'shell/widgets.cc',
        'shell/window_type_launcher.cc',
        'shell/window_type_launcher.h',
        '<(SHARED_INTERMEDIATE_DIR)/ui/gfx/gfx_resources.rc',
        '<(SHARED_INTERMEDIATE_DIR)/ui/ui_resources_standard/ui_resources_standard.rc',
        '<(SHARED_INTERMEDIATE_DIR)/ui/ui_resources/ui_resources.rc',
        '../ui/views/test/test_views_delegate.cc',
      ],
    },
  ],
}
