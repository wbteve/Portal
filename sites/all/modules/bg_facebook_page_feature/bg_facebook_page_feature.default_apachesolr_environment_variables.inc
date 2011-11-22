<?php
/**
 * @file
 * bg_facebook_page_feature.default_apachesolr_environment_variables.inc
 */

/**
 * Implementation of hook_default_apachesolr_environment_variable().
 */
function bg_facebook_page_feature_default_apachesolr_environment_variable() {
  $export = array();

  $name = new stdClass;
  $name->disabled = FALSE; /* Edit this to true to make a default name disabled initially */
  $name->api_version = 1;
  $name->env_id = 'solr';
  $name->name = 'apachesolr_search_excluded_types';
  $name->value = 'a:13:{s:6:"banner";s:6:"banner";s:13:"facebook_page";s:13:"facebook_page";s:17:"oai_oi_repository";s:17:"oai_oi_repository";s:14:"bvd_repository";s:14:"bvd_repository";s:6:"agenda";i:0;s:7:"article";i:0;s:4:"blog";i:0;s:7:"dossier";i:0;s:5:"flash";i:0;s:5:"media";i:0;s:4:"news";i:0;s:7:"webform";i:0;s:6:"iframe";i:0;}';
  $export['apachesolr_search_excluded_types'] = $name;

  return $export;
}
