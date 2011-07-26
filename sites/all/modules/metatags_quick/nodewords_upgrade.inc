<?php
/**
 * @file upgrade path from the D6 nodewords module
 * @author maxiorel <http://drupal.org/user/49016>
 * @author valthebald <http://drupal.org/user/239562>
 */

function metatags_quick_upgrade() {
  if (db_table_exists('nodewords')) {
    $form['description'] = array(
      '#type' => 'markup',
      '#markup' => '<p>' . t('This utility will convert the keywords and description from Nodewords table to the Meta tags quick table.') . '</p>'
    );
    $form['batch_size'] = array(
      '#type' => 'textfield',
      '#default_value' => '100',
      '#description' => t('How many nodes to process during one iteration'),
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Convert'),
    );
  }
  else {
    $form['description'] = array(
      '#markup' => t('Nodewords table not found. Please check your installation'),
    );
  }
  return $form;
}

function metatags_quick_upgrade_submit($form, &$form_state) {
  $batch = metatags_quick_upgrade_batch($form_state['values']['batch_size']);
  batch_set($batch);
}

function metatags_quick_upgrade_finished($success, $results, $operations) {
  if ($success) {
    drupal_set_message(t('@node_count results processed in @requests iterations.', array('@node_count' => $_SESSION['nodewords_upgrade_total'], '@requests' => count($results))));
  }
  else {
    $error_operation = reset($operations);
    drupal_set_message(t('An error occurred while processing @operation with arguments : @args', array('@operation' => $error_operation[0], '@args' => print_r($error_operation[0], TRUE))));
  }
}


function metatags_quick_upgrade_batch($batch_size) {
  if ($batch_size < 2) {
    $batch_size = 1;
  }
  $num_operations = metatags_quick_get_node_count();
  $_SESSION['nodewords_upgrade_total'] = $num_operations;
  $_SESSION['nodewords_upgrade_processed'] = 0;
  drupal_set_message(t('Converting metatags for @num nodes', array('@num' => $num_operations)));
  $operations = array();
  $nid_result = db_select('node', 'n')
    ->fields('n', array('nid'))
    ->execute();
  $nids = array();
  foreach ($nid_result as $node) {
    $nids[] = $node->nid;
    if (count($nids) >= $batch_size) {
      $operations[] = array('metatags_quick_convert_metatags', array($nids));
      $nids = array();
    }
  }
  if (count($nids)) {
    $operations[] = array('metatags_quick_convert_metatags', array($nids));
  }
  $batch = array(
    'operations' => $operations,
    'progress_message' => t('Completed :nodes_completed of :nodes_total', array(
      ':nodes_completed' => $_SESSION['nodewords_upgrade_processed'], ':nodes_total' => $_SESSION['nodewords_upgrade_total'])),
    'finished' => 'metatags_quick_upgrade_finished',
    'file' => drupal_get_path('module', 'metatags_quick') . '/nodewords_upgrade.inc',
  );
  return $batch;
}

function metatags_quick_convert_metatags($nids) {
  $processed_nodes = node_load_multiple($nids);
  $select = db_select('nodewords', 'n')
    ->fields('n', array('id', 'name', 'content'))
    ->condition('n.name', array('keywords', 'description'), 'IN')
    ->condition('n.id', $nids, 'IN')
    ->execute();
  foreach ($select as $row) {
    $keywords[$row->id][$row->name] = unserialize($row->content);
  }
  
  foreach ($processed_nodes as $node) {
    $_SESSION['nodewords_upgrade_total']++;
    $node_changed = FALSE;
    if (!empty($keywords[$node->nid]['keywords'])) {
      $node_changed = TRUE;
      $node->meta_keywords[$node->language][0]['metatags_quick'] = substr($keywords[$node->nid]['keywords']['value'], 0, 255);
    } 
    if (!empty($keywords[$node->nid]['description'])) {
      $node_changed = TRUE;
      $node->meta_description[$node->language][0]['metatags_quick'] = substr($keywords[$node->nid]['description']['value'], 0, 255);
    }
    if ($node_changed) {
      node_save($node);
    }
  }
}

function metatags_quick_get_node_count() {
  $query = 'SELECT count(*) as count FROM {node}';
  $result = db_query($query);
  foreach ($result as $record) {
    $pocet = $record->count;
  }
  return $pocet;
}