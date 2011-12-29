// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Boilerplate code was generated by http://burtonini.com/cgi/gobject.py

#include <gdk/gdkkeysyms.h>

#include "undo_view.h"

G_DEFINE_TYPE (GtkUndoView, gtk_undo_view, GTK_TYPE_TEXT_VIEW)

static void
gtk_undo_view_dispose(GObject *object) {
  GtkUndoView *uview = GTK_UNDO_VIEW(object);

  if(uview->undo_manager_) {
    g_object_unref(G_OBJECT(uview->undo_manager_));
    uview->undo_manager_ = NULL;
  }
  G_OBJECT_CLASS(gtk_undo_view_parent_class)->dispose(object);
}

static void
gtk_undo_view_undo(GtkUndoView *uview) {
  if(gtk_source_undo_manager_can_undo(uview->undo_manager_))
    gtk_source_undo_manager_undo(uview->undo_manager_);
}

static void
gtk_undo_view_redo(GtkUndoView *uview) {
  if(gtk_source_undo_manager_can_redo(uview->undo_manager_))
    gtk_source_undo_manager_redo(uview->undo_manager_);
}

static void
gtk_undo_view_class_init(GtkUndoViewClass *klass) {
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GtkBindingSet *binding_set;

  g_signal_new("undo",
      G_TYPE_FROM_CLASS(klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
      G_STRUCT_OFFSET(GtkUndoViewClass, undo),
      NULL,
      NULL,
      g_cclosure_marshal_VOID__VOID,
      G_TYPE_NONE,
      0);
  g_signal_new("redo",
      G_TYPE_FROM_CLASS(klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
      G_STRUCT_OFFSET(GtkUndoViewClass, redo),
      NULL,
      NULL,
      g_cclosure_marshal_VOID__VOID,
      G_TYPE_NONE,
      0);

  klass->undo = gtk_undo_view_undo;
  klass->redo = gtk_undo_view_redo;

  binding_set = gtk_binding_set_by_class(klass);
  gtk_binding_entry_add_signal(binding_set, GDK_z, GDK_CONTROL_MASK, "undo", 0);
  gtk_binding_entry_add_signal(binding_set, GDK_y, GDK_CONTROL_MASK, "redo", 0);
  gtk_binding_entry_add_signal(binding_set, GDK_z, GDK_CONTROL_MASK | GDK_SHIFT_MASK, "redo", 0);
  gtk_binding_entry_add_signal(binding_set, GDK_F14, 0, "undo", 0);

  object_class->dispose = gtk_undo_view_dispose;
}

static void
gtk_undo_view_init(GtkUndoView *self) {
}

GtkWidget*
gtk_undo_view_new(GtkTextBuffer *buffer) {
  GtkWidget *ret = g_object_new(GTK_TYPE_UNDO_VIEW, "buffer", buffer, NULL);
  GTK_UNDO_VIEW(ret)->undo_manager_ = gtk_source_undo_manager_new(GTK_TEXT_BUFFER(buffer));

  return ret;
}

