<?php

/**
 * @file
 * Theme functions for the Facet API module.
 */

/**
 * Themes a missing facet.
 */
function theme_facetapi_facet_missing($variables) {
  return t('Missing %field_name', array('%field_name' => $variables['field_name']));
}

/**
 * Themes the facet title.
 */
function theme_facetapi_title($variables) {
  return t('Filter by @title:', array('@title' => drupal_strtolower($variables['title'])));
}

/**
 * Themes a facet link with an optional count.
 */
function theme_facetapi_link_inactive($variables) {
  if (!empty($variables['count'])) {
    $variables['text'] .= ' ' . theme('facetapi_count', $variables);
  }
  return theme_link($variables);
}

/**
 * Themes the count for the facet item.  For example, this number shows how many
 * results will be returned after clicking on a link.
 */
function theme_facetapi_count($variables) {
  return '(' . check_plain($variables['count']) . ')';
}

/**
 * Themes a facet link for a value that is currently being searched.
 */
function theme_facetapi_link_active($variables) {
  $suffix = '';
  if (isset($variables['text'])) {
    if (empty($variables['options']['html'])) {
      $suffix = ' ' . check_plain($variables['text']);
    }
    else {
      $suffix  = ' ' . $variables['text'];
    }
  }
  $variables['text'] = '(-) ';
  return theme_link($variables) . $suffix;
}
