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
