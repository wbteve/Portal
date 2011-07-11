<?php

/**
 * @file
 * Block realm code and hook implementations.
 */

/**
 * Returns block information.
 */
function facetapi_get_block_info($realm_name = 'block') {
  $blocks = array();

  // Gets delta map, iterates over all enabled facets.
  $map = facetapi_get_delta_map();
  foreach (facetapi_get_searcher_info() as $searcher => $info) {
    // Gets cache settings for the searcher.
    $cache = variable_get('facetapi:block_cache:' . $searcher, DRUPAL_NO_CACHE);

    // Adds "Current Search" blocks.
    $delta = array_search($searcher . ':current_search', $map);
    $blocks[$delta] = array(
      'info' => 'Facet API: ' . $info['label'] . ' : ' . t('Current Search'),
      'cache' => $cache,
    );

    // Adds facet blocks.
    $facets = facetapi_get_enabled_facets($searcher, $realm_name);
    foreach ($facets as $facet_name => $facet) {

      // Gets the delta from the delta map.
      $string = $searcher . ':' . $realm_name . ':' . urlencode($facet_name);
      $delta = array_search($string, $map);

      // Defines the block.
      $blocks[$delta] = array(
        'info' => 'Facet API: ' . $info['label'] . ' : ' . $facet['label'],
        'cache' => $cache,
      );
    }
  }

  // Returns available blocks.
  return $blocks;
}

/**
 * Returns the content for a facet based on the delta.
 */
function facetapi_get_block($delta) {
  $builds = &drupal_static(__FUNCTION__, array());

  // Bails if delta is not mapped.
  $map = facetapi_get_delta_map();
  if (!isset($map[$delta])) {
    return;
  }

  // Extracts the searcher, realm name, and facet name from $delta.
  // Process the parts from the end in case the searcher includes a ':'.
  $parts = explode(':', $map[$delta]);
  $facet_name = array_pop($parts);
  $facet_name = urldecode($facet_name);
  $realm_name = array_pop($parts);
  $searcher = implode(':', $parts);

  // If we are viewing the current search block, set variable names accordingly.
  if (!$searcher && 'current_search' == $facet_name) {
    $searcher = $realm_name;
    $realm_name = $facet_name = FALSE;
  }

  // Bails if output is suppressed, such as if a search hasn't been executed.
  if (!$adapter = facetapi_adapter_load($searcher)) {
    return;
  }

  // If there is no realm, we are rendering the curent search block.
  if (FALSE === $realm_name && $adapter->searchExecuted()) {
    return array(
      'subject' => t('Current search'),
      'content' => $adapter->buildCurrentSearch(),
    );
  }

  // Bails if the output should be suppressed.
  if ($adapter->suppressOutput($realm_name)) {
    return;
  }

  // Builds and caches the entire realm per searcher / realm combination.
  $group = $searcher . ':' . $realm_name;
  if (!isset($builds[$group])) {
    $builds[$group] = facetapi_build_realm($searcher, $realm_name);
  }

  // Returns the individual block.
  if (isset($builds[$group][$facet_name])) {

    // Adds contextual links.
    $builds[$group][$facet_name]['#contextual_links'] = array(
      'facetapi' => array('admin/config/search/facetapi', array($searcher, $realm_name, $facet_name)),
    );

    // Returns the subject and content of the block.
    $variables = array('title' => $builds[$group][$facet_name]['#title']);
    return array(
      'subject' => theme('facetapi_title', $variables),
      'content' => $builds[$group][$facet_name]
    );
  }
}

/**
 * Returns a cached delta map of hashes to names.
 *
 * Sometimes our deltas are longer than 32 chars and need to be passed to hash().
 * Due to the block table's schema, deltas cannot be longer than 32 characters.
 * However, hashes are nasty as CSS IDs, so we can use the map to convert
 * the hashes back to a nicer value in facetapi_preprocess_block().
 *
 * @return
 *   An array containing the delta map.
 */
function facetapi_get_delta_map() {
  $map = &drupal_static(__FUNCTION__);
  if (NULL === $map) {
    if ($data = cache_get('facetapi:delta_map')) {
      $map = $data->data;
    }
    else {
      $map = array();

      // Calculates deltas for facets in the block realm.
      foreach (facetapi_get_searcher_info() as $searcher => $info) {

        // Maps current search block.
        $delta = $searcher . ':current_search';
        $map[facetapi_hash_delta($delta)] = $delta;

        // Maps facet deltas.
        // @todo - some other way to define realms that are block-like.
        foreach (array('block') as $realm_name) {
          foreach (facetapi_get_enabled_facets($searcher, $realm_name) as $facet) {
            $delta = $searcher . ':' . $realm_name . ':' . urlencode($facet['name']);
            $map[facetapi_hash_delta($delta)] = $delta;
          }
        }
      }

      // Caches the map so we don't have to do this repeatedly.
      cache_set('facetapi:delta_map', $map, 'cache', CACHE_TEMPORARY);
    }
  }

  return $map;
}

/**
 * Hashing code for deltas.
 *
 * @param $delta
 *   A string containing the delta.
 *
 * @return
 *   The hashed delta value.
 */
function facetapi_hash_delta($delta) {
  return (strlen($delta) <= 32) ? $delta : substr(drupal_hash_base64($delta), 0, 32);
}
