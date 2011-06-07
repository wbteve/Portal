<?php
/**
 * @file
 * Contains theme override functions and preprocess functions for the theme.
 *
 * ABOUT THE TEMPLATE.PHP FILE
 *
 *   The template.php file is one of the most useful files when creating or
 *   modifying Drupal themes. You can modify or override Drupal's theme
 *   functions, intercept or make additional variables available to your theme,
 *   and create custom PHP logic. For more information, please visit the Theme
 *   Developer's Guide on Drupal.org: http://drupal.org/theme-guide
 *
 * OVERRIDING THEME FUNCTIONS
 *
 *   The Drupal theme system uses special theme functions to generate HTML
 *   output automatically. Often we wish to customize this HTML output. To do
 *   this, we have to override the theme function. You have to first find the
 *   theme function that generates the output, and then "catch" it and modify it
 *   here. The easiest way to do it is to copy the original function in its
 *   entirety and paste it here, changing the prefix from theme_ to beeldgeluid_.
 *   For example:
 *
 *     original: theme_breadcrumb()
 *     theme override: beeldgeluid_breadcrumb()
 *
 *   where beeldgeluid is the name of your sub-theme. For example, the
 *   zen_classic theme would define a zen_classic_breadcrumb() function.
 *
 *   If you would like to override either of the two theme functions used in Zen
 *   core, you should first look at how Zen core implements those functions:
 *     theme_breadcrumbs()      in zen/template.php
 *     theme_menu_local_tasks() in zen/template.php
 *
 *   For more information, please visit the Theme Developer's Guide on
 *   Drupal.org: http://drupal.org/node/173880
 *
 * CREATE OR MODIFY VARIABLES FOR YOUR THEME
 *
 *   Each tpl.php template file has several variables which hold various pieces
 *   of content. You can modify those variables (or add new ones) before they
 *   are used in the template files by using preprocess functions.
 *
 *   This makes THEME_preprocess_HOOK() functions the most powerful functions
 *   available to themers.
 *
 *   It works by having one preprocess function for each template file or its
 *   derivatives (called template suggestions). For example:
 *     THEME_preprocess_page    alters the variables for page.tpl.php
 *     THEME_preprocess_node    alters the variables for node.tpl.php or
 *                              for node-forum.tpl.php
 *     THEME_preprocess_comment alters the variables for comment.tpl.php
 *     THEME_preprocess_block   alters the variables for block.tpl.php
 *
 *   For more information on preprocess functions and template suggestions,
 *   please visit the Theme Developer's Guide on Drupal.org:
 *   http://drupal.org/node/223440
 *   and http://drupal.org/node/190815#template-suggestions
 */


/**
 * Override or insert variables into the html templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("html" in this case.)
 */
function beeldgeluid_preprocess_html(&$variables, $hook) {
  if ($node = menu_get_object()) {
    $variables['node'] = $node;
  }
  
  if (isset($variables['node']->field_seo_code_head[$node->language])) {
    $variables['seo_code_head'] = $variables['node']->field_seo_code_head[$node->language][0]['value'];
  }
  else {
    $variables['seo_code_head'] = '';
  }

  if (isset($variables['node']->field_seo_code_body[$node->language])) {
    $variables['seo_code_body'] = $variables['node']->field_seo_code_body[$node->language][0]['value'];
  }
  else {
    $variables['seo_code_body'] = '';
  }
}

/**
 * Override or insert variables into the page templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("page" in this case.)
 */

function beeldgeluid_preprocess_page(&$variables, $hook) {
  global $language;
  
  $variables['page_title_tag'] = 'h1';
  
  
  // Check if the SEO header is set. If so, the page title must not be a <h1> because the SEO header will be <h1>.
  if (isset($variables['node']->field_seo_h1[$variables['node']->language][0]['safe_value'])) {
    $variables['page_title_tag'] = 'h2';
  };
  
  // $variables['theme_hook_suggestions'][] = 'page__'. $variables['node']->type;
  if (isset($variables['node']->type) && $variables['node']->type == 'blog') {
    $variables['page_title_tag'] = 'h2';
    if (!empty($variables['node']->field_blog[$language->language][0]['tid'])) {
      $term = taxonomy_term_load($variables['node']->field_blog[$language->language][0]['tid']);
      $variables['title'] = $term->name. ' blog';
      $variables['$head_title_array'][0] = $term->name. ' blog';
    }
    else {
      $variables['title'] = '';
      $variables['$head_title_array'][0] = '';
    }
  }
}

/**
 * Override or insert variables into the node templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("node" in this case.)
 */
/* -- Delete this line if you want to use this function
function beeldgeluid_preprocess_node(&$variables, $hook) {
  $variables['sample_variable'] = t('Lorem ipsum.');

  // Optionally, run node-type-specific preprocess functions, like
  // beeldgeluid_preprocess_node_page() or beeldgeluid_preprocess_node_story().
  $function = __FUNCTION__ . '_' . $variables['node']->type;
  if (function_exists($function)) {
    $function($variables, $hook);
  }
}
// */

/**
 * Override or insert variables into the comment templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("comment" in this case.)
 */
/* -- Delete this line if you want to use this function
function beeldgeluid_preprocess_comment(&$variables, $hook) {
  $variables['sample_variable'] = t('Lorem ipsum.');
}
// */

/**
 * Override or insert variables into the block templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("block" in this case.)
 */
function beeldgeluid_preprocess_block(&$variables, $hook) {
  if ($variables['block']->module == 'nodeblock') {
    $variables['elements']['#block']->subject = NULL;
  }
}

/**
 * Theme the way an 'all day' label will look.
 * In this theme, we return an empty string to disable the '(all day)' suffix. 
 */
function beeldgeluid_date_all_day_label() {
  return '';
}
