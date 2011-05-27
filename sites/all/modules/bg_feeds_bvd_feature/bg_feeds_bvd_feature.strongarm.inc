<?php
/**
 * @file
 * bg_feeds_bvd_feature.strongarm.inc
 */

/**
 * Implementation of hook_strongarm().
 */
function bg_feeds_bvd_feature_strongarm() {
  $export = array();

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_anonymous_bvd_repository';
  $strongarm->value = 0;
  $export['comment_anonymous_bvd_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_bvd_repository';
  $strongarm->value = '1';
  $export['comment_bvd_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_mode_bvd_repository';
  $strongarm->value = 1;
  $export['comment_default_mode_bvd_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_per_page_bvd_repository';
  $strongarm->value = '50';
  $export['comment_default_per_page_bvd_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_form_location_bvd_repository';
  $strongarm->value = 1;
  $export['comment_form_location_bvd_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_preview_bvd_repository';
  $strongarm->value = '1';
  $export['comment_preview_bvd_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_subject_field_bvd_repository';
  $strongarm->value = 1;
  $export['comment_subject_field_bvd_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'language_content_type_bvd_repository';
  $strongarm->value = '0';
  $export['language_content_type_bvd_repository'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_options_bvd_repository';
  $strongarm->value = array();
  $export['node_options_bvd_repository'] = $strongarm;

  return $export;
}
