<?php
/**
 * @file
 * bg_feeds_bvd_feature.feeds_importer_default.inc
 */

/**
 * Implementation of hook_feeds_importer_default().
 */
function bg_feeds_bvd_feature_feeds_importer_default() {
  $export = array();

  $feeds_importer = new stdClass;
  $feeds_importer->disabled = FALSE; /* Edit this to true to make a default feeds_importer disabled initially */
  $feeds_importer->api_version = 1;
  $feeds_importer->id = 'barend_en_van_dorp';
  $feeds_importer->config = array(
    'name' => 'Barend en Van Dorp',
    'description' => 'Importer for Barend en Van Dorp media',
    'fetcher' => array(
      'plugin_key' => 'FeedsFileFetcher',
      'config' => array(
        'allowed_extensions' => 'csv',
        'direct' => 0,
      ),
    ),
    'parser' => array(
      'plugin_key' => 'FeedsBVDCSVParser',
      'config' => array(
        'delimiter' => ',',
        'no_headers' => 0,
      ),
    ),
    'processor' => array(
      'plugin_key' => 'FeedsNodeProcessor',
      'config' => array(
        'content_type' => 'media',
        'expire' => '-1',
        'author' => 0,
        'mappings' => array(
          0 => array(
            'source' => 'expressie id',
            'target' => 'guid',
            'unique' => 1,
          ),
          1 => array(
            'source' => 'titel reeks',
            'target' => 'title',
            'unique' => FALSE,
          ),
          2 => array(
            'source' => 'bvd_file',
            'target' => 'field_media_file',
            'unique' => FALSE,
          ),
          3 => array(
            'source' => 'uitzend-datum',
            'target' => 'field_media_date:start',
            'unique' => FALSE,
          ),
          4 => array(
            'source' => 'beschrijving website',
            'target' => 'field_media_description',
            'unique' => FALSE,
          ),
          5 => array(
            'source' => 'bvd_source',
            'target' => 'field_media_source',
            'unique' => FALSE,
          ),
          6 => array(
            'source' => 'bvd_tags',
            'target' => 'field_media_tags',
            'unique' => FALSE,
          ),
          7 => array(
            'source' => 'afleveringsnummer',
            'target' => 'field_media_episode',
            'unique' => FALSE,
          ),
          8 => array(
            'source' => 'bvd_people',
            'target' => 'field_media_people',
            'unique' => FALSE,
          ),
          9 => array(
            'source' => 'bvd_length',
            'target' => 'field_media_length',
            'unique' => FALSE,
          ),
        ),
        'update_existing' => '0',
        'input_format' => 'plain_text',
      ),
    ),
    'content_type' => 'bvd_repository',
    'update' => 0,
    'import_period' => '-1',
    'expire_period' => 3600,
    'import_on_create' => 1,
    'process_in_background' => 0,
  );
  $export['barend_en_van_dorp'] = $feeds_importer;

  return $export;
}
