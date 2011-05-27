<?php
/**
 * @file
 * bg_feeds_oi_feature.strongarm.inc
 */

/**
 * Implementation of hook_strongarm().
 */
function bg_feeds_oi_feature_strongarm() {
  $export = array();

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_anonymous_oai_oi_repository';
  $strongarm->value = 0;
  $export['comment_anonymous_oai_oi_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_mode_oai_oi_repository';
  $strongarm->value = 1;
  $export['comment_default_mode_oai_oi_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_per_page_oai_oi_repository';
  $strongarm->value = '50';
  $export['comment_default_per_page_oai_oi_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_form_location_oai_oi_repository';
  $strongarm->value = 1;
  $export['comment_form_location_oai_oi_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_oai_oi_repository';
  $strongarm->value = '1';
  $export['comment_oai_oi_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_preview_oai_oi_repository';
  $strongarm->value = '1';
  $export['comment_preview_oai_oi_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_subject_field_oai_oi_repository';
  $strongarm->value = 1;
  $export['comment_subject_field_oai_oi_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'language_content_type_oai_oi_repository';
  $strongarm->value = '0';
  $export['language_content_type_oai_oi_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_options_oai_oi_repository';
  $strongarm->value = array();
  $export['node_options_oai_oi_repository'] = $strongarm;

  return $export;
}
