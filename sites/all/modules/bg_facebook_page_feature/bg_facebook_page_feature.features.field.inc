<?php
/**
 * @file
 * bg_facebook_page_feature.features.field.inc
 */

/**
 * Implementation of hook_field_default_fields().
 */
function bg_facebook_page_feature_field_default_fields() {
  $fields = array();

  // Exported field: 'node-facebook_page-field_referenced_content'
  $fields['node-facebook_page-field_referenced_content'] = array(
    'field_config' => array(
      'active' => '1',
      'cardinality' => '-1',
      'deleted' => '0',
      'entity_types' => array(),
      'field_name' => 'field_referenced_content',
      'foreign keys' => array(),
      'indexes' => array(),
      'module' => 'bg_reference',
      'settings' => array(
        'profile2_private' => FALSE,
      ),
      'translatable' => '1',
      'type' => 'bg_reference',
    ),
    'field_instance' => array(
      'bundle' => 'facebook_page',
      'default_value' => NULL,
      'deleted' => '0',
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'module' => 'bg_reference',
          'settings' => array(),
          'type' => 'bg_reference_facebook',
          'weight' => '0',
        ),
        'dossier' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
        'facebook' => array(
          'label' => 'hidden',
          'module' => 'bg_reference',
          'settings' => array(),
          'type' => 'bg_reference_facebook',
          'weight' => '0',
        ),
        'teaser' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'entity_type' => 'node',
      'field_name' => 'field_referenced_content',
      'label' => 'Referenced content',
      'required' => 1,
      'settings' => array(
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 0,
        'module' => 'bg_reference',
        'settings' => array(),
        'type' => 'bg_referencefield',
        'weight' => '1',
      ),
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Referenced content');

  return $fields;
}
