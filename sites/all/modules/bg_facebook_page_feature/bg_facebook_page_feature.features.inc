<?php
/**
 * @file
 * bg_facebook_page_feature.features.inc
 */

/**
 * Implementation of hook_ctools_plugin_api().
 */
function bg_facebook_page_feature_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => 1);
  }
}

/**
 * Implementation of hook_node_info().
 */
function bg_facebook_page_feature_node_info() {
  $items = array(
    'facebook_page' => array(
      'name' => t('Facebook page'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
