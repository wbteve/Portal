<?php

/**
 * @file
 * Administrative functions for Relation module.
 */

/**
 * List all relation predicates (page callback).
 */
function relation_list() {
  $relation = relation_entity_info();
  $field_ui = module_exists('field_ui');
  $headers = array(
    array('data' => t('Name'), 'width' => '40%'),
    array('data' => t('Operations'), 'colspan' => $field_ui ? '4' : '2'));
  $rows = array();
  foreach ($relation['relation']['bundles'] as $name => $bundle) {
    $url = 'admin/structure/relation/manage/' . $name;
    $label = $bundle['label'] ? $bundle['label'] : $name;
    $row = array(l($label, $url));
    $row[] = l(t('edit'), $url . '/edit');
    if ($field_ui) {
      $row[] =  l(t('manage fields'), $url . '/fields');
      $row[] =  l(t('display fields'), $url . '/display');
    }
    $row[] =  l(t('delete'), $url . '/delete');
    $rows[] = $row;
  }
  $output = array(
    '#theme' => 'table',
    '#header' => $headers,
    '#rows' => $rows,
  );
  return $output;
}

/**
 * Relation relation type bundle settings form.
 *
 * @param $predicate
 *   Relation type machine name. If this is not provided, assume that we're
 *   creating a new relation type.
 */
function relation_type_form($form, &$form_state, $predicate = array(), $op = 'edit') {
  $form['#write_record_keys'] = array();
  if ($predicate) {
    $relation_type = (object) $predicate;
    if ($relation_type) {
      $form['#write_record_keys'][] = 'predicate';
    }
  }
  if (empty($relation_type)) {
    $relation_type = (object) array(
      'predicate' => $predicate,
      'label' => '',
      'bundles' => array(),
      'directional' => FALSE,
      'transitive' => FALSE,
      'min_arity' => 2,
      'max_arity' => 2,
      'source_bundles' => array(),
      'target_bundles' => array(),
    );
  }
  $predicate = $relation_type->predicate;
  $form['predicate'] = array(
    '#type'          => 'machine_name',
    '#title'         => t('Machine Name'),
    '#description'   => t('Machine name (predicate) of the relation'),
    '#default_value' => $predicate,
    '#required'      => TRUE,
    '#disabled'      => $predicate,
    '#machine_name' => array(
      'exists' => 'relation_type_load',
    ),
  );
  $form['label'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Label'),
    '#description'   => t('Display name of the relation'),
    '#default_value' => $relation_type->label,
    '#required'      => TRUE,
  );
  $form['directional'] = array(
    '#type'           => 'checkbox',
    '#title'          => 'Directional',
    '#default_value'  => $relation_type->directional,
  );
  $form['transitive'] = array(
    '#type'           => 'checkbox',
    '#title'          => 'Transitive',
    '#default_value'  => $relation_type->transitive,
  );
  // these should probably be changed to numerical (validated) textfields.
  $options = array('2' => '2', '3' => '3', '4' => '4', '5' => '5', '8' => '8');
  $form['min_arity'] = array(
    '#type' => 'select',
    '#title' => t('Minimum Arity'),
    '#options' => $options,
    '#description' => t('Minimum number of entities joined by the relation (e.g. three siblings in one relation). <em>In nearly all cases you will want to leave this set to 2</em>.'),
    '#default_value' => $relation_type->min_arity ? $relation_type->min_arity : 2,
    '#states' => array(
      'disabled' => array(   // action to take.
        ':input[name="directional"]' => array('checked' => TRUE),
      ),
    ),
  );

  $options = array('2' => '2', '3' => '3', '4' => '4', '5' => '5', '8' => '8', '0' => t('Infinite'));
  $form['max_arity'] = array(
    '#type' => 'select',
    '#title' => t('Maximum Arity'),
    '#options' => $options,
    '#description' => t('Maximum number of entities joined by the relation. <em>In nearly all cases you will want to leave this set to 2</em>.'),
    '#default_value' => $relation_type->max_arity ? $relation_type->max_arity : 2,
    '#states' => array(
      'disabled' => array(   // action to take.
        ':input[name="directional"]' => array('checked' => TRUE),
      ),
    ),
  );
  $counter = 0;
  foreach (module_invoke_all('entity_info') as $entity_type => $entity) {
    $bundles[$entity['label']]["$entity_type:*"] = 'all ' . $entity['label'] . ' bundles';
    $counter += 2;
    if (isset($entity['bundles'])) {
      foreach ($entity['bundles'] as $bundle_id => $bundle) {
        $bundles[$entity['label']]["$entity_type:$bundle_id"] = $bundle['label'];
        $counter++;
      }
    }
  }
  $form['source_bundles'] = array(
    '#type'          => 'select',
    '#title'         => 'Available source bundles',
    '#options'       => $bundles,
    '#size'          => max(12, $counter),
    '#default_value' => $relation_type->source_bundles,
    '#multiple'      => TRUE,
    '#description'   => 'Bundles that are not selected will not be available as sources for directional, or end points of non-directional relations relations. Ctrl+click to select multiple. Note that selecting all bundles also include bundles not yet created for that entity type.',
  );
  $form['target_bundles'] = array(
    '#type'          => 'select',
    '#title'         => 'Available target bundles',
    '#options'       => $bundles,
    '#size'          => max(12, $counter),
    '#default_value' => $relation_type->target_bundles,
    '#multiple'      => TRUE,
    '#description'   => 'Bundles that are not selected will not be available as targets for directional relations. Ctrl+click to select multiple.',
    '#states' => array(
      '!visible' => array(   // action to take.
        ':input[name="directional"]' => array('checked' => FALSE),
      ),
    ),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );
  return $form;
}

/**
 * Submit data from bundle settings page.
 */
function relation_type_form_submit($form, &$form_state) {
  $predicate = $form_state['values']['predicate'];
  $min_arity = $form_state['values']['directional'] ? 2 : $form_state['values']['min_arity'];
  $max_arity = $form_state['values']['directional'] ? 2 : $form_state['values']['max_arity'];
  $record = array(
    'predicate'   => $predicate,
    'min_arity'   => $min_arity,
    'max_arity'   => $max_arity,
    'label' => $form_state['values']['label'],
    'directional' => $form_state['values']['directional'],
    'transitive' => $form_state['values']['transitive'],
    'source_bundles' => $form_state['values']['source_bundles'],
    'target_bundles' => $form_state['values']['target_bundles'],
  );
  relation_type_save($record, $form['#write_record_keys']);
  $form_state['redirect'] = "admin/structure/relation/edit/$predicate";

  drupal_set_message(t('The %predicate relation type has been saved.', array('%predicate' => $predicate)));
}

/**
 * Menu callback; deletes a single relation type.
 */
function relation_type_delete_confirm($form, &$form_state, $predicate) {
  $form['predicate'] = array('#type' => 'value', '#value' => $predicate->predicate);
  $form['label'] = array('#type' => 'value', '#value' => $predicate->label);

  $message = t('Are you sure you want to delete the %label relation type?', array('%label' => $predicate->label));
  $caption = '';

  $num_relations = relation_query()->predicate($predicate->predicate)->count()->execute();
  if ($num_relations) {
    $caption .= '<p>' . format_plural($num_relations, 'The %label relation type is used by 1 relation on your site. If you remove this relation type, you will not be able to edit  %label relations and they may not display correctly.', 'The %label relation type is used by @count relations on your site. If you remove %label, you will not be able to edit %label relations and they may not display correctly.', array('%label' => $predicate->label, '@count' => $num_relations)) . '</p>';
  }

  $caption .= '<p>' . t('This action cannot be undone.') . '</p>';

  return confirm_form($form, $message, 'admin/structure/relation', $caption, t('Delete'));
}

/**
 * Process relation type delete confirm submissions.
 */
function relation_type_delete_confirm_submit($form, &$form_state) {
  relation_type_delete($form_state['values']['predicate']);

  $t_args = array('%label' => $form_state['values']['label']);
  drupal_set_message(t('The %label relation type has been deleted.', $t_args));
  watchdog('relation', 'Deleted the %name relation type.', $t_args, WATCHDOG_NOTICE);

  // TODO: relation_types_rebuild() ?;
  menu_rebuild();

  $form_state['redirect'] = 'admin/structure/relation';
  return;
}
