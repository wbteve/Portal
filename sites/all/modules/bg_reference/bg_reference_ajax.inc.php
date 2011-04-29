<?php
function _bg_reference_autocomplete($bundle, $string = '') {
  $matches = array();
  if ($string) {
    $result = db_select('node')
      ->fields('node', array('nid', 'vid', 'title'))
      ->condition('type', $bundle)
      ->condition('title', db_like($string) . '%', 'LIKE')
      ->range(0, 10)
      ->execute();
    foreach ($result as $node) {
      $matches["{$node->title} [nid:{$node->nid}] [vid:{$node->vid}]"] = check_plain($node->title);
    }
  }

  drupal_json_output($matches);
}

function _bg_reference_ajax($element) {
  
}
/*
'ref_banner' => array(
      '#type'               => 'textfield',
      '#title'              => t('Banner'),
      '#attributes'         => array('class' => array('bg_reference_banner')),
      '#autocomplete_path'  => 'bg-reference/autocomplete/' . CONTENT_TYPE_BANNER,
    ),
    'ref_event' => array(
      '#type'               => 'textfield',
      '#title'              => t('Event'),
      '#attributes'         => array('class' => array('bg_reference_event')),
      '#autocomplete_path'  => 'bg-reference/autocomplete/' . CONTENT_TYPE_EVENT,
    ),
    'ref_search_result' => array(
      '#type'               => 'textfield',
      '#title'              => t('Search result'),
      '#attributes'         => array('class' => array('bg_reference_search_result')),
      // '#autocomplete_path'  => 'bg-reference/autocomplete/' . CONTENT_TYPE_SEARCH_RESULT,
    ),*/