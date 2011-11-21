<?php
/**
 * @file
 * bg_facebook_page_feature.strongarm.inc
 */

/**
 * Implementation of hook_strongarm().
 */
function bg_facebook_page_feature_strongarm() {
  $export = array();

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_anonymous_facebook_page';
  $strongarm->value = 0;
  $export['comment_anonymous_facebook_page'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_mode_facebook_page';
  $strongarm->value = 1;
  $export['comment_default_mode_facebook_page'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_per_page_facebook_page';
  $strongarm->value = '50';
  $export['comment_default_per_page_facebook_page'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_facebook_page';
  $strongarm->value = '1';
  $export['comment_facebook_page'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_form_location_facebook_page';
  $strongarm->value = 1;
  $export['comment_form_location_facebook_page'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_preview_facebook_page';
  $strongarm->value = '1';
  $export['comment_preview_facebook_page'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_subject_field_facebook_page';
  $strongarm->value = 1;
  $export['comment_subject_field_facebook_page'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'language_content_type_facebook_page';
  $strongarm->value = '0';
  $export['language_content_type_facebook_page'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_options_facebook_page';
  $strongarm->value = array(
    0 => 'status',
  );
  $export['node_options_facebook_page'] = $strongarm;

  return $export;
}
