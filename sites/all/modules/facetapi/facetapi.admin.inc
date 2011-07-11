<?php

/**
 * @file
 * Facet API administrative settings forms.
 */

/**
 * Realm settings form.
 *
 * @param $form
 *
 * @param &$form_state
 *
 * @param $searcher
 *   A string containing the machine readable name of the searcher.
 * @param $realm_name
 *   A string containing the machine readable name of the realm.
 *
 * @return
 *   An array containing the form.
 *
 * @ingroup forms
 */
function facetapi_realm_settings_form($form, &$form_state, $searcher, $realm_name) {

  // Instantiates adapter, loads realm.
  $adapter = facetapi_adapter_load($searcher);
  $realm = facetapi_realm_load($realm_name);
  $form['#facetapi'] = array(
    'adapter' => $adapter,
    'realm' => $realm,
    'facet_info' => facetapi_get_facet_info($searcher),
  );

  $form['description'] = array(
    '#prefix' => '<div class="facetapi-realm-description">',
    '#markup' => filter_xss_admin($realm['description']),
    '#suffix' => "</div>\n",
  );

  $form['performance'] = array(
    '#prefix' => '<div class="facetapi-performance-note">',
    '#markup' => t('For performance reasons, you should only enable facets that you intend to have available to users on the search page.'),
    '#suffix' => "</div>\n",
  );

  $form['table'] = array(
    '#theme' => 'facetapi_realm_settings_table',
    '#facetapi' => &$form['#facetapi'],
  );

  // Builds "enabled_facets" options.
  $options = $default_value = array();
  foreach ($form['#facetapi']['facet_info'] as $facet_name => $facet) {
    $settings = $adapter->getFacet($facet)->getSettings($realm);

    if ($realm['sortable']) {
      $form['#facetapi']['facet_info'][$facet_name]['weight'] = $settings->settings['weight'];
      $form['table']['weight:' . $facet_name] = array(
        '#type' => 'select',
        '#options' => drupal_map_assoc(range(-50, 50)),
        '#default_value' => $settings->settings['weight'],
        '#attributes' => array('class' => array('facetapi-facet-weight')),
      );
    }

    $options[$facet_name] = '';
    $default_value[$facet_name] = (!$settings->enabled) ? 0 : $facet_name;
  }

  // Sorts by the weight appended above.
  uasort($form['#facetapi']['facet_info'], 'drupal_sort_weight');

  $form['table']['enabled_facets'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => $default_value,
  );

  // Checks whether block caching is enabled, sets description accordingly.
  if (!$disabled = (module_implements('node_grants') || !variable_get('block_cache', FALSE))) {
    $description = t('Configure the appropriate cache setting for facet blocks.');
  }
  else {
    $description = t(
      'To enable block caching, visit the <a href="@performance-page">performance page</a>.',
      array('@performance-page' => url('admin/config/development/performance', array('query' => array('destination' => $_GET['q']))))
    );
  }
  $form['block_cache'] = array(
    '#type' => 'select',
    '#access' => ('block' == $realm_name),
    '#title' => t('Block cache settings'),
    '#disabled' => $disabled,
    '#options' => array(
      DRUPAL_NO_CACHE => t('Do not cache'),
      DRUPAL_CACHE_PER_ROLE | DRUPAL_CACHE_PER_PAGE => t('Per role'),
      DRUPAL_CACHE_PER_USER | DRUPAL_CACHE_PER_PAGE => t('Per user'),
    ),
    '#default_value' => variable_get('facetapi:block_cache:' . $searcher, DRUPAL_NO_CACHE),
    '#description' => $description,
  );

  $form['actions'] = array(
    '#type' => 'actions',
    '#weight' => 20,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  $form['#submit'][] = 'facetapi_realm_settings_form_submit';

  return $form;
}

/**
 * Processes facetapi_realm_settings_form form submissions.
 */
function facetapi_realm_settings_form_submit($form, &$form_state) {
  $success = TRUE;

  // Pulls variables for code readability.
  $adapter = $form['#facetapi']['adapter'];
  $realm = $form['#facetapi']['realm'];
  $facet_info = $form['#facetapi']['facet_info'];

  // Builds settings object, saves to database.
  foreach ($facet_info as $facet_name => $facet) {
    $settings = $adapter->getFacet($facet)->getSettings($realm);
    $settings->enabled = empty($form_state['values']['enabled_facets'][$facet_name]) ? 0 : 1;
    $weight = $realm['sortable'] ? $form_state['values']['weight:' . $facet_name] : 0;
    $settings->settings['weight'] = $weight;
    if (FALSE === ctools_export_crud_save('facetapi', $settings)) {
      drupal_set_message(t('Error saving settings.'), 'error');
      $success = FALSE;
    }
  }

  // Sets message if all configurations were saved.
  if ($success) {
    drupal_set_message(t('The configuration options have been saved.'));
    if ('block' == $form['#facetapi']['realm']['name']) {
      drupal_set_message(t(
        'To enable or arrange the facet blocks, visit the <a href="@block-page">blocks administration page</a>.',
        array('@block-page' => url('admin/structure/block', array('query' => array('destination' => $_GET['q']))))
      ));
    }
  }

  // Saves block cache settings, clears block cache if setting was changed.
  if (isset($form_state['values']['block_cache'])) {
    $name = 'facetapi:block_cache:' . $adapter->getSearcher();
    $original = variable_get($name, DRUPAL_NO_CACHE);
    variable_set($name, $form_state['values']['block_cache']);
    if ($original != $form_state['values']['block_cache']) {
      cache_clear_all(NULL, 'cache_block');
    }
  }

  // Clears static cache and delta map delta map.
  cache_clear_all('facetapi:delta_map', 'cache');
}

/**
 * Themes a realm settings table.
 */
function theme_facetapi_realm_settings_table($variables) {
  $output = '';

  // Adds CSS to ensure that the dropbutton looks nice.
  $path = drupal_get_path('module', 'facetapi');
  drupal_add_css($path . '/facetapi.css', array('weight' => CSS_THEME - 1));

  // Gets variales for code readability.
  $searcher = $variables['element']['#facetapi']['adapter']->getSearcher();
  $realm = $variables['element']['#facetapi']['realm'];

  $header = array(
    'enabled' => array('data' => t('Enabled')),
    'label' => array('data' => t('Facet')),
    'operations' => array('data' => t('Operations')),
  );
  if ($realm['sortable']) {
    $header['weight'] = array('data' => t('Weight'));
  }

  // Builds field options.
  $rows = array();
  $facet_info = $variables['element']['#facetapi']['facet_info'];
  foreach ($facet_info as $facet_name => $facet) {

    // Builds array of operations to use in the dropbutton.
    $operations = array();
    $operations[] = array(
      'title' => t('Configure display'),
      'href' => facetapi_get_settings_path($searcher, $realm['name'], $facet_name, 'edit')
    );
    if ($facet['dependency plugins']) {
      $operations[] = array(
        'title' => t('Configure dependencies'),
        'href' => facetapi_get_settings_path($searcher, $realm['name'], $facet_name, 'dependencies')
      );
    }
    $operations[] = array(
      'title' => t('Export configuration'),
      'href' => facetapi_get_settings_path($searcher, $realm['name'], $facet_name, 'export')
    );

    // Builds rows.
    $rows[$facet_name] = array(
      'class' => $realm['sortable'] ? array('draggable') : array(),
      'data' => array(
        'indexed' => array(
          'data' => drupal_render($variables['element']['enabled_facets'][$facet_name]),
        ),
        'label' => array(
          'data' => check_plain($facet['label']) . "<div class='description'>" . filter_xss($facet['description']) . '</div>',
        ),
        'operations' => array(
          'class' => 'facetapi-operations',
          'data' => theme('links__ctools_dropbutton', array(
            'links' => $operations,
            'attributes' => array(
              'class' => array('inline', 'links', 'actions', 'horizontal', 'right')
            ),
          )),
        ),
      ),
    );
    if ($realm['sortable']) {
      $rows[$facet_name]['data']['weight'] = array(
        'data' => drupal_render($variables['element']['weight:' . $facet_name]),
      );
    }
  }

  if ($realm['sortable']) {
    drupal_add_tabledrag('facetapi-ui-realm-settings', 'order', 'sibling', 'facetapi-facet-weight');
  }
  $output .= drupal_render_children($variables['element']);
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'facetapi-ui-realm-settings')));

  return $output;
}

/**
 * Returns path to search page.
 */
function facetapi_get_settings_path($searcher, $realm_name, $facet_name, $op) {
  $path_parts = array('admin', 'config', 'search', 'facetapi');
  $path_parts[] = $searcher;
  $path_parts[] = $realm_name;
  $path_parts[] = $facet_name;
  $path_parts[] = $op;
  return join('/', $path_parts);
}

/**
 * Returns array of widget definitions available to the facet.
 *
 * @param array $realm
 *   The realm definition.
 * @param array $facet
 *   The facet definition.
 *
 * @return array
 *   An array of available widget plugins.
 */
function facetapi_get_widgets(array $realm, array $facet) {
  $plugins = array();

  // Iterates over all defined plugins.
  foreach (ctools_get_plugins('facetapi', 'widgets') as $id => $plugin) {
    $append = TRUE;

    // Initializes the requirements if empty.
    $plugin['handler'] += array(
      'requirements' => array(
        'facetapi_requirement_realm_property' => array('element type' => 'links')
      ),
    );

    // Requirements fail if at least one requirements callback returns FALSE.
    foreach ($plugin['handler']['requirements'] as $callback => $options) {
      if (!call_user_func($callback, $realm, $facet, $options)) {
        $append = FALSE;
        break;
      }
    }

    // Appends plugin if no requiremetns failed.
    if ($append) {
      $plugins[$id] = $plugin;
    }
  }

  return $plugins;
}

/**
 * Checks realm property requirement.
 */
function facetapi_requirement_realm_property(array $realm, array $facet, array $options) {
  return facetapi_requirement_property($realm, $options);
}

/**
 * Checks facet property requirement.
 */
function facetapi_requirement_facet_property(array $realm, array $facet, array $options) {
  return facetapi_requirement_property($facet, $options);
}

/**
 * Checks the equality of a property.
 */
function facetapi_requirement_property(array $definition, array $options) {
  $passed = TRUE;
  foreach ($options as $key => $requirement) {
    $condition = $definition[$key];
    if (is_array($requirement)) {
      if (array_intersect_key($condition, $requirement) != $requirement) {
        $passed = FALSE;
        break;
      }
    }
    else {
      if ($requirement != $condition) {
        $passed = FALSE;
        break;
      }
    }
  }
  return $passed;
}

/**
 * Facet display settings form.
 *
 * @param $form
 *
 * @param &$form_state
 *
 * @param FacetapiAdapter $adapter
 *   The adapter the settings apply to.
 * @param array $realm
 *   The realm definition.
 * @param array $facet
 *   The facet definition.
 *
 * @return
 *   An array containing the form.
 *
 * @ingroup forms
 */
function facetapi_facet_settings_form($form, &$form_state, FacetapiAdapter $adapter, array $realm, array $facet) {
  $path = drupal_get_path('module', 'facetapi');
  drupal_add_css($path . '/facetapi.css', array('weight' => CSS_THEME - 1));
  drupal_add_js($path . '/facetapi.admin.js');
  ctools_include('plugins');

  // We have to set the title due to contextual link magic.
  // @see http://drupal.org/node/1147588#comment-4428940
  drupal_set_title(t('Configure facet display'));

  // Captures variables and settings for code readability.
  $searcher = $adapter->getSearcher();
  $facet_settings = $adapter->getFacet($facet)->getSettings($realm);
  $global_settings = $adapter->getFacet($facet)->getSettings();

  // Stores required items that are related to the facet being configured in
  // this form. The excluded values are items submitted through the form that
  // should not be stores as configuration options.
  $form['#facetapi'] = array(
    'adapter' => $adapter,
    'realm' => $realm,
    'facet' => $facet,
    'sort_info' => array(),
    'excluded_values' => array(
      'form_build_id', 'form_token', 'form_id', 'op', 'submit', 'submit_list',
      'settings__active_tab',
    ),
  );

  ////
  ////
  //// Widget settings
  ////
  ////

  $form['widget'] = array(
    '#type' => 'fieldset',
    '#title' => t('Widget settings'),
  );

  // Builds select options for widgets, allows widgets to add settings.
  $widget_options = array();
  foreach (facetapi_get_widgets($realm, $facet) as $id => $plugin) {
    $widget_options[$id] = $plugin['handler']['label'];
    $class = $plugin['handler']['class'];
    $plugin = new $class($id, $realm, $adapter->getFacet($facet), $facet_settings);
    $plugin->settingsForm($form, $form_state);
  }

  $form['widget']['widget'] = array(
    '#type' => 'select',
    '#title' => t('Display widget'),
    '#default_value' => $facet_settings->settings['widget'],
    '#options' => $widget_options,
    '#weight' => -10,
    '#description' => t('Select the display widget used to render this facet.'),
  );

  ////
  ////
  //// Sort settings
  ////
  ////

  $form['widget']['sort'] = array(
    '#prefix' => '<div class="facetapi-sort-table">',
    '#suffix' => '</div>',
    '#weight' => -10,
  );

  $form['widget']['sort']['table'] = array(
    '#theme' => 'facetapi_sort_settings_table',
    '#facetapi' => &$form['#facetapi'],
  );

  // Initializes sorts with default settings, orders by default weight. Ordering
  // the weights allows us to iterate over them in order when building the
  // form elements in the foreach() loop below.
  $sort_weight = $facet_settings->settings['sort_weight'];
  $sort_order = $facet_settings->settings['sort_order'];
  foreach (facetapi_get_sort_info() as $sort_name => $sort_info) {
    $weight = (isset($sort_weight[$sort_name])) ? $sort_weight[$sort_name] : 0;
    $form['#facetapi']['sort_info'][$sort_name] = $sort_info;
    $form['#facetapi']['sort_info'][$sort_name]['weight'] = $weight;
  }

  // Orders the sorts by the default weights set above.
  uasort($form['#facetapi']['sort_info'], 'drupal_sort_weight');

  // Builds checkbox options and weight dropboxes.
  $sort_options = array();
  foreach ($form['#facetapi']['sort_info'] as $sort_name => $sort) {
    $sort_options[$sort_name] = '';

    $order_default = (isset($sort_order[$sort_name])) ? $sort_order[$sort_name] : SORT_ASC;
    $form['widget']['sort']['table']['sort_order:' . $sort_name] = array(
      '#type' => 'select',
      '#title' => '',
      '#options' => array(
        SORT_DESC => t('Descending'),
        SORT_ASC => t('Ascending'),
      ),
      '#default_value' => $order_default,
    );

    $weight_default = (isset($sort_weight[$sort_name])) ? $sort_weight[$sort_name] : 0;
    $form['widget']['sort']['table']['sort_weight:' . $sort_name] = array(
      '#type' => 'weight',
      '#title' => '',
      '#delta' => 50,
      '#default_value' => $weight_default,
      '#attributes' => array('class' => array('facetapi-sort-weight')),
    );
  }

  $form['widget']['sort']['table']['active_sorts'] = array(
    '#type' => 'checkboxes',
    '#options' => $sort_options,
    '#default_value' => $facet_settings->settings['active_sorts'],
  );

  $form['widget']['empty'] = array(
    '#prefix' => '<div class="facetapi-empty-setting">',
    '#suffix' => '</div>',
    '#weight' => 10,
  );

  $empty_options = array();
  foreach (ctools_get_plugins('facetapi', 'empty_behaviors') as $id => $plugin) {
    $empty_options[$id] = $plugin['handler']['label'];
    $class = $plugin['handler']['class'];
    $plugin = new $class($facet_settings);
    $plugin->settingsForm($form, $form_state);
  }

  $form['widget']['empty']['empty_behavior'] = array(
    '#type' => 'select',
    '#title' => t('Empty facet behavior'),
    '#default_value' => $facet_settings->settings['empty_behavior'],
    '#options' => $empty_options,
    '#weight' => -10,
    '#description' => t('The action to take when a facet has no items.'),
  );

  ////
  ////
  //// Global settings
  ////
  ////

  $form['global'] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => t('Global settings'),
    '#description' => t('The configuration options below apply to this facet across <em>all</em> realms.'),
  );

  $all_options = array(
    FACETAPI_OPERATOR_AND => t('AND'),
    FACETAPI_OPERATOR_OR => t('OR'),
  );
  $options = array_intersect_key($all_options, array_filter($facet['allowed operators']));
  $form['global']['operator'] = array(
    '#type' => 'radios',
    '#access' => count($options) > 1,
    '#title' => t('Operator'),
    '#prefix' => '<div class="facetapi-global-setting">',
    '#suffix' => '</div>',
    '#default_value' => $global_settings->settings['operator'],
    '#options' => $options,
    '#description' => t('AND filters are exclusive and narrow the result set. OR filters are inclusive and widen the result set.')
  );

  $form['global']['hard_limit'] = array(
    '#type' => 'select',
    '#title' => t('Hard limit'),
    '#prefix' => '<div class="facetapi-global-setting">',
    '#suffix' => '</div>',
    '#default_value' => $global_settings->settings['hard_limit'],
    '#options' => drupal_map_assoc(array(100, 75, 50, 40, 30, 20, 15, 10, 5, 3)),
    '#description' => t('Display no more than this number of facet items.')
  );

  $form['global']['flatten'] = array(
    '#type' => 'radios',
    '#access' => $facet['hierarchy callback'],
    '#title' => t('Flatten hierarchy'),
    '#prefix' => '<div class="facetapi-global-setting">',
    '#suffix' => '</div>',
    '#default_value' => $global_settings->settings['flatten'],
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#description' => t('Do not process hierarchical relationships and display facet items as a flat list.'),
  );

  $form['global']['facet_missing'] = array(
    '#type' => 'radios',
    '#access' => $facet['facet missing allowed'] && $adapter->supportsFacetMissing(),
    '#title' => t('Add facet for missing values'),
    '#prefix' => '<div class="facetapi-global-setting">',
    '#suffix' => '</div>',
    '#default_value' => $global_settings->settings['facet_missing'],
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#description' => t('Adds an extra facet matching documents with no value at all for this field.')
  );

  ////
  ////
  //// Finalizes form
  ////
  ////

  $form['actions'] = array(
    '#type' => 'actions',
    '#weight' => 20,
  );

  // Gets destination from query string which is set when the page is navigated
  // to via a contextual link. Builds messages based on where user came from.
  if (isset($_GET['destination']) && !url_is_external($_GET['destination'])) {
    $submit_text = t('Save and go back to search page');
    $cancel_title = t('Return to the search page without saving configuration changes.');
    $url = drupal_parse_url($_GET['destination']);
  }
  else {
    $submit_text = t('Save configuration');
    $cancel_title = t('Return to the realm settings page without saving configuration changes.');
    $url = array();
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => $submit_text,
  );

  // Do not show the button if the page was navigated to via a contextual link
  // because it would redirect the user back to the search page.
  $form['actions']['submit_realm'] = array(
    '#type' => 'submit',
    '#access' => (!$url),
    '#value' => t('Save and go back to realm settings'),
  );

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => (!$url) ? $adapter->getPath($realm['name']) : $url['path'],
    '#options' => (!$url) ? array() : array('query' => $url['query']),
    '#attributes' => array('title' => $cancel_title),
  );

  // Adds submit handler, allow the realm and adapter to alter the form.
  $form['#submit'][] = 'facetapi_facet_settings_form_submit';
  if ($realm['settings callback']) {
    $realm['settings callback']($form, $form_state);
  }
  $adapter->settingsForm($form, $form_state);

  return $form;
}

/**
 * Themes a sort settings table.
 */
function theme_facetapi_sort_settings_table($variables) {
  $output = '';

  // Gets variales for code readability.
  $searcher = $variables['element']['#facetapi']['adapter']->getSearcher();

  $header = array(
    'active' => array(),
    'label' => array('data' => t('Sort')),
    'order' => array('data' => t('Order')),
    'weight' => array('data' => t('Weight')),
  );

  // Builds field options.
  $rows = array();
  $sort_info = $variables['element']['#facetapi']['sort_info'];
  foreach ($sort_info as $sort_name => $sort) {
    $rows[$sort_name] = array(
      'class' => array('draggable'),
      'data' => array(
        'active' => array(
          'data' => drupal_render($variables['element']['active_sorts'][$sort_name]),
        ),
        'label' => array(
          'data' => check_plain($sort['label']) . "<div class='description'>" . filter_xss($sort['description']) . '</div>',
        ),
        'order' => array(
          'data' => drupal_render($variables['element']['sort_order:' . $sort_name]),
        ),
        'weight' => array(
          'data' => drupal_render($variables['element']['sort_weight:' . $sort_name]),
        ),
      ),
    );
  }

  drupal_add_tabledrag('facetapi-ui-sort-settings', 'order', 'sibling', 'facetapi-sort-weight');
  $output .= drupal_render_children($variables['element']);
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'facetapi-ui-sort-settings')));

  return $output;
}

/**
 * Processes facetapi_facet_settings_form submissions.
 */
function facetapi_facet_settings_form_submit($form, &$form_state) {

  // Pulls variables for code readability.
  $adapter = $form['#facetapi']['adapter'];
  $realm = $form['#facetapi']['realm'];
  $facet = $form['#facetapi']['facet'];
  $global_values = $form_state['values']['global'];
  unset($form_state['values']['global']);

  // Loads settings, saves all form values as settings other than excluded.
  $facet_settings = $adapter->getFacet($facet)->getSettings($realm);
  $facet_settings->settings = array_merge($facet_settings->settings, array_diff_key(
    $form_state['values'],
    array_flip($form['#facetapi']['excluded_values'])
  ));

  // Converts sort_weight:* and sort_order:* settings to associative arrays.
  facetapi_combine_settings('sort_weight', $facet_settings);
  facetapi_combine_settings('sort_order', $facet_settings);

  $global_settings = $adapter->getFacet($facet)->getSettings();
  foreach ($global_values as $key => $value) {
    $global_settings->settings[$key] = $value;
  }

  $success = TRUE;
  if (FALSE === ctools_export_crud_save('facetapi', $facet_settings)) {
    drupal_set_message(t('Error saving configuration options.'), 'error');
    $success = FALSE;
  }
  if (FALSE === ctools_export_crud_save('facetapi', $global_settings)) {
    drupal_set_message(t('Error saving configuration options.'), 'error');
    $success = FALSE;
  }

  // Sets message if both sets of configurations were saved.
  if ($success) {
    drupal_set_message(t('The configuration options have been saved.'));
  }

  // Redirects back to the realm settings page if necessary.
  $clicked = $form_state['clicked_button']['#value'];
  if (t('Save and go back to realm settings') == $clicked) {
    $form_state['redirect'] = $adapter->getPath($realm['name']);
  }
}

/**
 * Combines settings named "setting:*" into a single array of settings.
 *
 * This makes it easier to process settings at runtime.
 *
 * @param $setting_name
 *   A string containing the name of the setting.
 * @param $settings
 *   An object containing the settings loaded from CTools.
 */
function facetapi_combine_settings($setting_name, $settings) {
  $start = strlen($setting_name) + 1;
  $settings->settings[$setting_name] = array();
  foreach ($settings->settings as $name => $value) {
    if (0 === strpos($name, $setting_name . ':')) {
      $key = substr($name, $start);
      unset($settings->settings[$name]);
      $settings->settings[$setting_name][$key] = $value;
    }
  }
}

/**
 * Facet dependency settings form.
 *
 * @param $form
 *
 * @param &$form_state
 *
 * @param FacetapiAdapter $adapter
 *   The adapter the settings apply to.
 * @param array $realm
 *   The realm definition.
 * @param array $dependencies
 *   An array of dependencies.
 *
 * @return
 *   An array containing the form.
 *
 * @ingroup forms
 */
function facetapi_facet_dependencies_form($form, &$form_state, FacetapiAdapter $adapter, array $realm, array $dependencies) {

  // We have to set the title due to contextual link magic.
  // @see http://drupal.org/node/1147588#comment-4428940
  drupal_set_title(t('Configure facet dependencies'));

  // Adds Facet API settings, excluded values aren't saved.
  $form['#facetapi'] = array(
    'adapter' => $adapter,
    'realm' => $realm,
    'settings' => FALSE,
    'defaults' => array(),
  );

  $form['description'] = array(
    '#prefix' => '<div class="facetapi-realm-description">',
    '#markup' => t('Dependencies are conditions that must be met in order for the facet to be processed by the server and displayed to the user. Hiding facets via the core block system or through the Drupal forms API will not prevent the server from processing the facets.'),
    '#suffix' => "</div>\n",
  );

  $form['plugins'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 10,
  );

  // Iterates over plugins, adds settings as vertical tabs.
  $plugins = ctools_get_plugins('facetapi', 'dependencies');
  foreach ($dependencies as $plugin) {
    // Only gets settings once.
    if (!$form['#facetapi']['settings']) {
      $settings = $adapter->getFacet($plugin->getFacet())->getSettings();
      $form['#facetapi']['settings'] = $settings;
    }

    // Initializes vertical tab.
    $id = $plugin->getId();
    $form[$id] = array(
      '#type' => 'fieldset',
      '#title' => check_plain($plugins[$id]['handler']['label']),
      '#group' => 'plugins',
    );

    // Allows plugin to add settings to the form, adds defaults.
    $plugin->settingsForm($form, $form_state);
    $form['#facetapi']['defaults'] += $plugin->getDefaultSettings();

    // Removes vertical tab if nothing was added.
    if (!element_children($form[$id])) {
      unset($form[$id]);
    }
  }

  $form['actions'] = array(
    '#type' => 'actions',
    '#weight' => 20,
  );

  // Gets destination from query string which is set when the page is navigated
  // to via a contextual link. Builds messages based on where user came from.
  if (isset($_GET['destination']) && !url_is_external($_GET['destination'])) {
    $submit_text = t('Save and go back to search page');
    $cancel_title = t('Return to the search page without saving configuration changes.');
    $url = drupal_parse_url($_GET['destination']);
  }
  else {
    $submit_text = t('Save configuration');
    $cancel_title = t('Return to the realm settings page without saving configuration changes.');
    $url = array();
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => $submit_text,
  );

  // Do not show the button if the page was navigated to via a contextual link
  // because it would redirect the user back to the search page.
  $form['actions']['submit_realm'] = array(
    '#type' => 'submit',
    '#access' => (!$url),
    '#value' => t('Save and go back to realm settings'),
  );

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => (!$url) ? $adapter->getPath($realm['name']) : $url['path'],
    '#options' => (!$url) ? array() : array('query' => $url['query']),
    '#attributes' => array('title' => $cancel_title),
  );

  $form['#submit'][] = 'facetapi_facet_dependencies_form_submit';

  return $form;
}

/**
 * Processes facetapi_facet_settings_form submissions.
 */
function facetapi_facet_dependencies_form_submit($form, &$form_state) {
  $success = TRUE;

  // Pulls variables for code readability.
  $adapter = $form['#facetapi']['adapter'];
  $realm = $form['#facetapi']['realm'];
  $settings = $form['#facetapi']['settings'];
  $defaults = $form['#facetapi']['defaults'];

  // Gets dependency settings from form.
  $values = array_intersect_key($form_state['values'], $defaults);
  $settings->settings['dependencies'] = $values + $defaults;

  // Writes the settings to the database.
  if (FALSE === ctools_export_crud_save('facetapi', $settings)) {
    drupal_set_message(t('Error saving configuration options.'), 'error');
    $success = FALSE;
  }

  // Sets message if both sets of configurations were saved.
  if ($success) {
    drupal_set_message(t('The configuration options have been saved.'));
  }

  // Redirects back to the realm settings page if necessary.
  $clicked = $form_state['clicked_button']['#value'];
  if (t('Save and go back to realm settings') == $clicked) {
    $form_state['redirect'] = $adapter->getPath($realm['name']);
  }
}

/**
 * Export form, wrapper around ctools_export_form().
 *
 * @param $form
 *
 * @param &$form_state
 *
 * @param $settings
 *
 * @see ctools_export_form()
 */
function facetapi_export_form($form, &$form_state, $adapter, $realm, $facet) {

  // Adds link to get back to list page.
  $form['back'] = array(
    '#type' => 'link',
    '#title' => t('Go back to realm settings page'),
    '#href' => $adapter->getPath($realm['name']),
  );

  // Gets export fieldset from ctools_export_form().
  $title = t('Export configuration');
  $settings = $adapter->getFacet($facet)->getSettings($realm);
  $export = ctools_export_crud_export('facetapi', $settings);
  $form = ctools_export_form($form, $form_state, $export, $title);

  // Adds settings and returns form.
  $form['#settings'] = $settings;
  return $form;
}
