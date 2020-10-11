#pragma once

#include <qwidget.h>

#include <map>
#include <optional>  // optional
#include <vector>    // vector, erase_if

#include "position.hpp"
#include "tileset_tab.hpp"
#include "types.hpp"
#include "vector_map.hpp"

namespace Ui {
class tileset_content_page;
}

namespace tactile::gui {

class tab_data final
{
 public:
  tab_data() = default;

  void erase(tileset_id id)
  {
    m_tabs.erase(id);
  }

  void add(tileset_id id, tileset_tab* tab)
  {
    m_tabs.emplace(id, tab);
  }

  void set_cached_index(int index) noexcept
  {
    m_cachedIndex = index;
  }

  [[nodiscard]] auto cached_index() const noexcept -> std::optional<int>
  {
    if (m_cachedIndex != -1) {
      return m_cachedIndex;
    } else {
      return std::nullopt;
    }
  }

  [[nodiscard]] auto is_empty() const noexcept -> bool
  {
    return m_tabs.empty();
  }

  [[nodiscard]] auto contains(tileset_id id) const -> bool
  {
    return m_tabs.contains(id);
  }

  auto begin() noexcept
  {
    return m_tabs.begin();
  }

  auto begin() const noexcept
  {
    return m_tabs.begin();
  }

  auto end() noexcept
  {
    return m_tabs.end();
  }

  auto end() const noexcept
  {
    return m_tabs.end();
  }

 private:
  std::map<tileset_id, tileset_tab*> m_tabs{};
  int m_cachedIndex{-1};
};

/**
 * @class tileset_content_page
 *
 * @brief Represents the contents of a tileset widget when there are at least
 * one available tileset.
 *
 * @since 0.1.0
 *
 * @headerfile tileset_content_page.hpp
 */
class tileset_content_page final : public QWidget
{
  Q_OBJECT

 public:
  /**
   * @brief Creates a `tileset_content_page` instance.
   *
   * @param parent a pointer to the parent widget.
   *
   * @since 0.1.0
   */
  explicit tileset_content_page(QWidget* parent = nullptr);

  ~tileset_content_page() noexcept override;

  /**
   * @brief Adds a tileset tab that represents the supplied tileset.
   *
   * @pre `map` must not have been added before.
   * @pre `id` must not have been added before.
   *
   * @param map the ID associated with the map that owns the tileset.
   * @param id the ID associated with the tileset that will be added.
   * @param tileset the tileset that will be added.
   *
   * @since 0.1.0
   */
  void add_tileset(map_id map, tileset_id id, const core::tileset& tileset);

  /**
   * @brief Removes the tileset tab associated with the specified tileset.
   *
   * @pre `id` must be associated with an existing tileset tab.
   *
   * @param id the ID associated with the tileset tab which will be removed.
   * @param notify `true` if the `ui_removed_tileset` signal should be emitted;
   * `false` otherwise.
   *
   * @since 0.1.0
   */
  void remove_tileset(tileset_id id, bool notify = true);

  /**
   * @brief Indicates whether or not there are any tileset tabs.
   *
   * @return `true` if there are no tileset tabs; `false` otherwise.
   *
   * @since 0.1.0
   */
  [[nodiscard]] auto empty() const -> bool;

 signals:
  void switch_to_empty_page();

  void switch_to_content_page();

  void ui_requested_tileset();

  void ui_selected_tileset(tileset_id id);

  void ui_removed_tileset(tileset_id id);

  void tileset_selection_changed(core::position topLeft,
                                 core::position bottomRight);

 public slots:
  void select_map(map_id map);

 private:
  Ui::tileset_content_page* m_ui{};
  std::optional<map_id> m_currentMap;
  std::map<map_id, tab_data> m_tabData;
  bool m_switchingMap{false};  ///< Used to know when to store tab indices

  void switch_to(map_id map);

  /**
   * @brief Returns the tab associated with the specified index.
   *
   * @param index the index of the desired tab.
   *
   * @return a pointer to the found tab; `nullptr` if no tab was found.
   *
   * @since 0.1.0
   */
  [[nodiscard]] auto tab_from_index(int index) -> tileset_tab*;

  /**
   * @brief Returns the index of the tab associated with the specified index.
   *
   * @param id the ID associated with the desired tab.
   *
   * @return the index of the tab associated with the ID; `std::nullopt` if no
   * such tab was found.
   *
   * @since 0.1.0
   */
  [[nodiscard]] auto index_of(tileset_id id) const -> std::optional<int>;

  [[nodiscard]] auto current_tab() const -> const tab_data&
  {
    return m_tabData.at(m_currentMap.value());
  }

  [[nodiscard]] auto current_tab() -> tab_data&
  {
    return m_tabData.at(m_currentMap.value());
  }

 private slots:
  void handle_remove_tab(int index);

  void handle_tab_changed(int index);

  void handle_tab_clicked(int index);
};

}  // namespace tactile::gui
