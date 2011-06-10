<?php
/**
 * @file
 * bg_feeds_oi_feature.features.inc
 */

/**
 * Implementation of hook_ctools_plugin_api().
 */
function bg_feeds_oi_feature_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "feeds" && $api == "feeds_importer_default") {
    return array("version" => 1);
  }
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => 1);
  }
}

/**
 * Implementation of hook_node_info().
 */
function bg_feeds_oi_feature_node_info() {
  $items = array(
    'oai_oi_repository' => array(
      'name' => t('OAI OI repository'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Titel'),
      'help' => '',
    ),
  );
  return $items;
}
