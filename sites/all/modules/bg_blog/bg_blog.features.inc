<?php
/**
 * @file
 * bg_blog.features.inc
 */

/**
 * Implementation of hook_ctools_plugin_api().
 */
function bg_blog_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => 1);
  }
}

/**
 * Implementation of hook_views_api().
 */
function bg_blog_views_api() {
  list($module, $api) = func_get_args();
  if ($module == "views" && $api == "views_default") {
    return array("version" => 3.0);
  }
}

/**
 * Implementation of hook_node_info().
 */
function bg_blog_node_info() {
  $items = array(
    'blog' => array(
      'name' => t('blog'),
      'base' => 'node_content',
      'description' => t('Blog post'),
      'has_title' => '1',
      'title_label' => t('Titel'),
      'help' => '',
    ),
  );
  return $items;
}
