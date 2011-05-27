<?php
/**
 * @file
 * bg_feeds_oi_feature.feeds_importer_default.inc
 */

/**
 * Implementation of hook_feeds_importer_default().
 */
function bg_feeds_oi_feature_feeds_importer_default() {
  $export = array();

  $feeds_importer = new stdClass;
  $feeds_importer->disabled = FALSE; /* Edit this to true to make a default feeds_importer disabled initially */
  $feeds_importer->api_version = 1;
  $feeds_importer->id = 'oai';
  $feeds_importer->config = array(
    'name' => 'OAI OI',
    'description' => 'OAI OpenImages importer',
    'fetcher' => array(
      'plugin_key' => 'FeedsOAIOIHTTPParser',
      'config' => array(
        'auto_detect_feeds' => FALSE,
        'use_pubsubhubbub' => FALSE,
        'last_fetched_timestamp' => '',
        'earliest_timestamp' => '',
        'granularity' => '',
      ),
    ),
    'parser' => array(
      'plugin_key' => 'FeedsOAIOIParser',
      'config' => array(),
    ),
    'processor' => array(
      'plugin_key' => 'FeedsNodeProcessor',
      'config' => array(
        'content_type' => 'media',
        'expire' => '-1',
        'author' => 0,
        'mappings' => array(
          0 => array(
            'source' => 'attributionURL',
            'target' => 'url',
            'unique' => 0,
          ),
          1 => array(
            'source' => 'guid',
            'target' => 'guid',
            'unique' => 1,
          ),
          2 => array(
            'source' => 'title',
            'target' => 'title',
            'unique' => FALSE,
          ),
          3 => array(
            'source' => 'attributionURL',
            'target' => 'field_media_source_url:url',
            'unique' => FALSE,
          ),
          4 => array(
            'source' => 'computedSource',
            'target' => 'field_media_file',
            'unique' => FALSE,
          ),
          5 => array(
            'source' => 'abstract',
            'target' => 'field_media_description',
            'unique' => FALSE,
          ),
          6 => array(
            'source' => 'date',
            'target' => 'field_media_date:start',
            'unique' => FALSE,
          ),
          7 => array(
            'source' => 'subject',
            'target' => 'field_media_tags',
            'unique' => FALSE,
          ),
          8 => array(
            'source' => 'importSource',
            'target' => 'field_media_source',
            'unique' => FALSE,
          ),
          9 => array(
            'source' => 'license',
            'target' => 'field_media_license',
            'unique' => FALSE,
          ),
        ),
        'update_existing' => '2',
        'input_format' => 'plain_text',
      ),
    ),
    'content_type' => 'oai_oi_repository',
    'update' => 0,
    'import_period' => '-1',
    'expire_period' => 3600,
    'import_on_create' => 0,
    'process_in_background' => 0,
  );
  $export['oai'] = $feeds_importer;

  return $export;
}
