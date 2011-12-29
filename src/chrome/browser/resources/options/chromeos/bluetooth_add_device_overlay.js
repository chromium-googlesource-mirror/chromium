// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

cr.define('options', function() {
  const OptionsPage = options.OptionsPage;

  /**
   * Encapsulated handling of the Bluetooth options page.
   * @constructor
   */
  function BluetoothOptions() {
    OptionsPage.call(this,
                     'bluetooth',
                     templateData.bluetoothOptionsPageTabTitle,
                     'bluetooth-options');
  }

  cr.addSingletonGetter(BluetoothOptions);

  BluetoothOptions.prototype = {
    __proto__: OptionsPage.prototype,

    /**
     * The list of available (unpaired) bluetooth devices.
     * @type {DeletableItemList}
     * @private
     */
    deviceList_: null,

    /** @inheritDoc */
    initializePage: function() {
      OptionsPage.prototype.initializePage.call(this);
      this.createDeviceList_();

      $('bluetooth-add-device-cancel-button').onclick = function(event) {
        OptionsPage.closeOverlay();
      };

      var self = this;
      $('bluetooth-add-device-apply-button').onclick = function(event) {
        var device = self.deviceList_.selectedItem;
        var address = device.address;
        chrome.send('updateBluetoothDevice', [address, 'connect']);
        OptionsPage.closeOverlay();
      };

      $('bluetooth-add-device-apply-button').onmousedown = function(event) {
        // Prevent 'blur' event, which would reset the list selection,
        // thereby disabling the apply button.
        event.preventDefault();
      };
    },

    /**
     * Creates, decorates and initializes the bluetooth device list.
     * @private
     */
    createDeviceList_: function() {
      this.deviceList_ = $('bluetooth-unpaired-devices-list');
      options.system.bluetooth.BluetoothDeviceList.decorate(this.deviceList_);
      this.deviceList_.autoExpands = true;
    }
  };

  // Export
  return {
    BluetoothOptions: BluetoothOptions
  };
});
