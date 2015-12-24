/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef LIST_H_
#define LIST_H_
#include <gtkmm.h>
#include "../../../lib/Type.h"
#include "../../../lib/Exception.h"
#include "../../Model/ListItem.h"
#include "../../../lib/str_replace.h"
#include <libintl.h>

template<typename TItem, typename TWrapper>
class View_Gtk_Element_List : public Gtk::TreeView {
public:
	struct TreeModel : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> text;
		Gtk::TreeModelColumn<TItem*> relatedRule;
		Gtk::TreeModelColumn<TWrapper*> relatedScript;
		Gtk::TreeModelColumn<bool> is_other_entries_marker;
		Gtk::TreeModelColumn<bool> is_renamable;
		Gtk::TreeModelColumn<bool> is_renamable_real;
		Gtk::TreeModelColumn<bool> is_editable;
		Gtk::TreeModelColumn<bool> is_sensitive;
		Gtk::TreeModelColumn<bool> is_activated;
		Gtk::TreeModelColumn<bool> is_toplevel;
		Gtk::TreeModelColumn<Pango::EllipsizeMode> ellipsize;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;

		TreeModel(){
			this->add(name);
			this->add(text);
			this->add(relatedRule);
			this->add(relatedScript);
			this->add(is_other_entries_marker);
			this->add(is_renamable);
			this->add(is_renamable_real);
			this->add(is_editable);
			this->add(is_activated);
			this->add(is_sensitive);
			this->add(is_toplevel);
			this->add(icon);
			this->add(ellipsize);
		}
	};
	TreeModel treeModel;
	Glib::RefPtr<Gtk::TreeStore> refTreeStore;
	Gtk::CellRendererPixbuf pixbufRenderer;
	Gtk::CellRendererToggle toggleRenderer;
	Gtk::CellRendererText textRenderer;
	Gtk::TreeViewColumn mainColumn;
	Pango::EllipsizeMode ellipsizeMode;

	View_Gtk_Element_List()
		: ellipsizeMode(Pango::ELLIPSIZE_NONE)
	{
		refTreeStore = Gtk::TreeStore::create(treeModel);
		this->set_model(refTreeStore);

		this->append_column(this->mainColumn);
		this->mainColumn.pack_start(pixbufRenderer, false);
		this->mainColumn.add_attribute(pixbufRenderer.property_pixbuf(), treeModel.icon);
		this->mainColumn.pack_start(toggleRenderer, false);
		this->mainColumn.add_attribute(toggleRenderer.property_sensitive(), treeModel.is_sensitive);
		toggleRenderer.set_visible(false);
		this->mainColumn.add_attribute(toggleRenderer.property_active(), treeModel.is_activated);
		this->mainColumn.pack_start(this->textRenderer, true);
		this->mainColumn.add_attribute(this->textRenderer.property_markup(), treeModel.text);
		this->mainColumn.add_attribute(this->textRenderer.property_editable(), treeModel.is_renamable);
		this->mainColumn.set_spacing(10);

		this->mainColumn.add_attribute(this->textRenderer.property_ellipsize(), treeModel.ellipsize);

		this->set_headers_visible(false);
		this->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
		this->set_rubber_banding(true);
	}

	void addListItem(View_Model_ListItem<TItem, TWrapper> const& listItem, std::map<ViewOption, bool> const& options, Gtk::Window& window) {
		if (!listItem.isVisible && !options.at(VIEW_SHOW_HIDDEN_ENTRIES)) {
			return;
		}
		if (listItem.entryPtr == NULL && !options.at(VIEW_GROUP_BY_SCRIPT)) {
			return;
		}
		if (listItem.is_placeholder && !options.at(VIEW_SHOW_PLACEHOLDERS)) {
			return;
		}
		Gtk::TreeIter entryRow;
		if (listItem.parentEntry) {
			try {
				entryRow = this->refTreeStore->append(this->getIterByRulePtr(listItem.parentEntry)->children());
			} catch (ItemNotFoundException const& e) {
				return; // this usually happens when there's a visible entry below a hidden submenu. Just don't show it in this case.
			}
		} else if (listItem.parentScript && options.at(VIEW_GROUP_BY_SCRIPT)) {
			entryRow = this->refTreeStore->append(this->getIterByScriptPtr(listItem.parentScript)->children());
		} else {
			entryRow = this->refTreeStore->append();
		}

		Glib::RefPtr<Gdk::Pixbuf> icon;
		std::string outputName = escapeXml(listItem.name);
		if (!listItem.is_placeholder) {
			outputName = "<b>" + outputName + "</b>";
		}
		if (options.at(VIEW_SHOW_DETAILS)) {
			outputName += "\n<small>";
			if (listItem.scriptPtr != NULL) {
				outputName += gettext("script");
			} else if (listItem.is_submenu) {
				outputName += gettext("submenu");
			} else if (listItem.is_placeholder) {
				outputName += gettext("placeholder");
			} else {
				outputName += gettext("menuentry");
			}
			if (listItem.scriptName != "") {
				outputName += std::string(" / ") + gettext("script: ") + escapeXml(listItem.scriptName);
			}

			if (listItem.defaultName != "" && listItem.name != listItem.defaultName) {
				outputName += std::string(" / ") + gettext("default name: ") + escapeXml(listItem.defaultName);
			}

			if (listItem.options.find("_deviceName") != listItem.options.end()) {
				outputName += escapeXml(Glib::ustring(" / ") + gettext("Partition: ") + listItem.options.at("_deviceName"));
			}

			outputName += "</small>";
		}

		if (listItem.scriptPtr != NULL) {
			icon = window.render_icon_pixbuf(Gtk::Stock::FILE, options.at(VIEW_SHOW_DETAILS) ? Gtk::ICON_SIZE_LARGE_TOOLBAR : Gtk::ICON_SIZE_MENU);
		} else if (listItem.is_submenu) {
			icon = window.render_icon_pixbuf(Gtk::Stock::DIRECTORY, options.at(VIEW_SHOW_DETAILS) ? Gtk::ICON_SIZE_LARGE_TOOLBAR : Gtk::ICON_SIZE_MENU);
		} else if (listItem.is_placeholder) {
			icon = window.render_icon_pixbuf(Gtk::Stock::FIND, options.at(VIEW_SHOW_DETAILS) ? Gtk::ICON_SIZE_LARGE_TOOLBAR : Gtk::ICON_SIZE_MENU);
		} else {
			icon = window.render_icon_pixbuf(Gtk::Stock::EXECUTE, options.at(VIEW_SHOW_DETAILS) ? Gtk::ICON_SIZE_LARGE_TOOLBAR : Gtk::ICON_SIZE_MENU);
		}

		if (listItem.isModified) {
			outputName = "<i>" + outputName + "</i>";
		}

		(*entryRow)[this->treeModel.name] = listItem.name;
		(*entryRow)[this->treeModel.text] = outputName;
		(*entryRow)[this->treeModel.is_activated] = listItem.isVisible;
		(*entryRow)[this->treeModel.relatedRule] = listItem.entryPtr;
		(*entryRow)[this->treeModel.relatedScript] = listItem.scriptPtr;
		(*entryRow)[this->treeModel.is_renamable] = false;
		(*entryRow)[this->treeModel.is_renamable_real] = !listItem.is_placeholder && listItem.scriptPtr == NULL;
		(*entryRow)[this->treeModel.is_editable] = listItem.isEditable;
		(*entryRow)[this->treeModel.is_sensitive] = listItem.scriptPtr == NULL;
		(*entryRow)[this->treeModel.is_toplevel] = listItem.parentEntry == NULL;
		(*entryRow)[this->treeModel.icon] = icon;
		(*entryRow)[this->treeModel.ellipsize] = ellipsizeMode;
	}

	Gtk::TreeModel::iterator getIterByRulePtr(TItem* rulePtr, const Gtk::TreeRow* parentRow = NULL) const {
		const Gtk::TreeNodeChildren children = parentRow ? parentRow->children() : this->refTreeStore->children();
		for (Gtk::TreeModel::const_iterator iter = children.begin(); iter != children.end(); iter++) {
			if ((*iter)[this->treeModel.relatedRule] == rulePtr)
				return iter;
			try {
				return this->getIterByRulePtr(rulePtr, &**iter); //recursively search for the treeview item
			} catch (ItemNotFoundException const& e) {
				//(ignore ItemNotFoundException exception)
			}
		}
		throw ItemNotFoundException("rule not found", __FILE__, __LINE__);
	}

	Gtk::TreeModel::iterator getIterByScriptPtr(TWrapper* scriptPtr) const {
		const Gtk::TreeNodeChildren children = this->refTreeStore->children();
		for (Gtk::TreeModel::const_iterator iter = children.begin(); iter != children.end(); iter++) {
			if ((*iter)[this->treeModel.relatedScript] == scriptPtr) {
				return iter;
			}
		}
		throw ItemNotFoundException("script not found", __FILE__, __LINE__);
	}

	void setRuleName(TItem* rule, std::string const& newName){
		Gtk::TreeModel::iterator iter = this->getIterByRulePtr(rule);
		(*iter)[this->treeModel.name] = newName;
	}


	void selectRule(TItem* rule, bool startEdit) {
		try {
			this->get_selection()->select(this->getIterByRulePtr(rule));
			if (startEdit) {
				this->set_cursor(this->refTreeStore->get_path(this->getIterByRulePtr(rule)), *this->get_column(0), true);
			}
		} catch (ItemNotFoundException const& e) {
			// do nothing
		}
	}

	void selectRules(std::list<TItem*> rules) {
		this->get_selection()->unselect_all();
		for (typename std::list<TItem*>::iterator iter = rules.begin(); iter != rules.end(); iter++) {
			try {
				this->get_selection()->select(this->getIterByRulePtr(*iter));
			} catch (ItemNotFoundException const& e) {
				// be fault-tolerant
			}
		}
	}

	void setEntryVisibility(TItem* entry, bool value) {
		(*this->getIterByRulePtr(entry))[this->treeModel.is_activated] = value;
	}

	std::list<TItem*> getSelectedRules() {
		std::list<TItem*> rules;
		std::vector<Gtk::TreeModel::Path> pathes = this->get_selection()->get_selected_rows();
		for (std::vector<Gtk::TreeModel::Path>::iterator iter = pathes.begin(); iter != pathes.end(); iter++) {
			TItem* rptr = (*this->refTreeStore->get_iter(*iter))[this->treeModel.relatedRule];
			rules.push_back(rptr);
		}

		return rules;
	}
};


#endif /* LIST_H_ */
