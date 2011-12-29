// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/models/list_model.h"

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace ui {

class FooItem {
 public:
  explicit FooItem(int id) : id_(id) {}

  int id() const { return id_; }

 private:
  int id_;
  DISALLOW_COPY_AND_ASSIGN(FooItem);
};

class ListModelTest : public testing::Test,
                      public ListModelObserver {
 public:
  ListModelTest()
      : added_count_(0),
        removed_count_(0),
        changed_count_(0) {
  }

  void ExpectCountsEqual(int added_count,
                         int removed_count,
                         int changed_count) {
    EXPECT_EQ(added_count, added_count_);
    EXPECT_EQ(removed_count, removed_count_);
    EXPECT_EQ(changed_count, changed_count_);
  }

  void ClearCounts() {
    added_count_ = removed_count_ = changed_count_ = 0;
  }

  // ListModelObserver implementation:
  virtual void ListItemsAdded(int start, int count) OVERRIDE {
    added_count_ += count;
  }
  virtual void ListItemsRemoved(int start, int count) OVERRIDE {
    removed_count_ += count;
  }
  virtual void ListItemsChanged(int start, int count) OVERRIDE {
    changed_count_ += count;
  }

 private:
  int added_count_;
  int removed_count_;
  int changed_count_;

  DISALLOW_COPY_AND_ASSIGN(ListModelTest);
};

TEST_F(ListModelTest, Add) {
  ListModel<FooItem> model;
  model.AddObserver(this);

  // Append FooItem(0)
  model.Add(new FooItem(0));
  ExpectCountsEqual(1, 0, 0);

  // Append FooItem(1)
  model.Add(new FooItem(1));
  ExpectCountsEqual(2, 0, 0);

  // Insert FooItem(2) at position 0
  model.AddAt(0, new FooItem(2));
  ExpectCountsEqual(3, 0, 0);

  // Total 3 items in mode.
  EXPECT_EQ(3, model.item_count());

  // First one should be FooItem(2), followed by FooItem(0) and FooItem(1)
  EXPECT_EQ(2, model.item_at(0)->id());
  EXPECT_EQ(0, model.item_at(1)->id());
  EXPECT_EQ(1, model.item_at(2)->id());
}

TEST_F(ListModelTest, Remove) {
  ListModel<FooItem> model;
  model.AddObserver(this);

  model.Add(new FooItem(0));
  model.Add(new FooItem(1));
  model.Add(new FooItem(2));

  ClearCounts();

  // Remove item at index 1 from model and release memory.
  model.DeleteAt(1);
  ExpectCountsEqual(0, 1, 0);

  EXPECT_EQ(2, model.item_count());
  EXPECT_EQ(0, model.item_at(0)->id());
  EXPECT_EQ(2, model.item_at(1)->id());

  // Remove all items from model and delete them.
  model.DeleteAll();
  ExpectCountsEqual(0, 3, 0);
}

TEST_F(ListModelTest, RemoveAll) {
  ListModel<FooItem> model;
  model.AddObserver(this);

  scoped_ptr<FooItem> foo0(new FooItem(0));
  scoped_ptr<FooItem> foo1(new FooItem(1));
  scoped_ptr<FooItem> foo2(new FooItem(2));

  model.Add(foo0.get());
  model.Add(foo1.get());
  model.Add(foo2.get());

  ClearCounts();

  // Remove all items and scoped_ptr above would release memory.
  model.RemoveAll();
  ExpectCountsEqual(0, 3, 0);
}

TEST_F(ListModelTest, FakeUpdate) {
  ListModel<FooItem> model;
  model.AddObserver(this);

  model.Add(new FooItem(0));
  model.Add(new FooItem(1));
  model.Add(new FooItem(2));

  ClearCounts();

  model.NotifyItemsChanged(0, 1);
  ExpectCountsEqual(0, 0, 1);

  model.NotifyItemsChanged(1, 2);
  ExpectCountsEqual(0, 0, 3);
}

}  // namespace ui
