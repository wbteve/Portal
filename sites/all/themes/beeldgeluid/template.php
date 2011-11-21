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


function beeldgeluid_breadcrumb($variables) {
  $breadcrumb = $variables['breadcrumb'];

  if (!empty($breadcrumb)) {
    // Override first crumb if it is a link
    if(strpos($breadcrumb[0], '<a ') !== FALSE) {
      $breadcrumb[0] = l(t('Home'), '', array('attributes' => array('class' => array('first'))));
    }

    $last_element_key = count($breadcrumb) -1;
    if($last_element_key > 0) {
      $breadcrumb[$last_element_key] = '<span class="last">' . $breadcrumb[$last_element_key] . '</span>';
    }

    // Provide a navigational heading to give context for breadcrumb links to
    // screen-reader users. Make the heading invisible with .element-invisible.
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';

    $output .= '<div class="breadcrumb">' . implode('/', $breadcrumb) . '</div>';
    return $output;
  }
}

/**
 * Override or insert variables into the html templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("html" in this case.)
 */
function beeldgeluid_preprocess_html(&$variables, $hook) {
  if (arg(0) == 'node' && arg(2)) {
    foreach ($variables['classes_array'] as $key => $class) {
      if (strpos($class, 'node-type-') === 0) {
        $variables['classes_array'][$key] = $class . '-' . check_plain(arg(2));
      }
    }
  }

  $variables['seo_code_head'] = '';
  $variables['seo_code_body'] = '';
  if ($node = menu_get_object()) {
    if (isset($node->field_seo_code_head[$node->language])) {
      $variables['seo_code_head'] = $node->field_seo_code_head[$node->language][0]['value'];
    }
    if (isset($node->field_seo_code_body[$node->language])) {
      $variables['seo_code_body'] = $node->field_seo_code_body[$node->language][0]['value'];
    }
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
  global $user;

  array_unshift($variables['theme_hook_suggestions'], 'page__'.$variables['node']->type);

  // Hide admin menu in embed-mode.
  if (end($variables['theme_hook_suggestions']) == 'page__node__embed') {
    module_invoke('admin_menu', 'suppress');
  }

  // Hide tabs for normal users
  $show_tabs = FALSE;
  foreach($user->roles as $roleid => $rolename){
    if($roleid != 2 && arg(0) != 'search'){
      $show_tabs = TRUE;
      break;
    }
  }

  if ($user->uid == 1) {
    $show_tabs = TRUE;
  }



  $variables['page_title_tag'] = 'h1';
  // Code that should only run when a node is being displayed.
  if (isset($variables['node'])) {
    // Check if the SEO header is set. If so, the page title must not be a <h1> because the SEO header will be <h1>.
    if (isset($variables['node']->field_seo_h1[$variables['node']->language])) {
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

  // If page is search results
  if(arg(0) == 'search') {
    $display_type = arg(1) == 'list' ? arg(1) : 'block';

    // Do not show tabs
    $show_tabs = FALSE;

    $adapter = facetapi_adapter_load('apachesolr@solr');
    drupal_set_title($adapter->getSearchKeys());

    $variables['search_info'] = '<p class="search-info-top">' . t('Search results for') . '</p>' .
      '<p class="search-info-bottom">' . t('@result_count results', array('@result_count' => $adapter->getResultCount()));

    $variables['search_advanced_info'] = '<p class="search-advanced-link"><a href="http://zoeken.beeldengeluid.nl" target="_blank">Duik nog dieper in onze archieven.</a></p> <p class="search-advanced-more-info">U zoekt nu binnen deze website en de media-items die we online kunnen vertonen. Ga naar <a href="http://zoeken.beeldengeluid.nl">zoeken.beeldengeluid.nl</a> voor uitgebreid zoeken in het hele Beeld en Geluid archief en direct bestellen van fragmenten.</p>';

    $variables['search_display_switch'] = array(
      '#markup' => l(t('Block view'), 'search/site/' . $adapter->getSearchKeys(), array(
          'query' => $adapter->getParams(),
          'attributes' => array(
          	'class' => array('search-display-switch-btn', 'search-display-switch-btn-block', ($display_type == 'list' ? '' : 'search-display-switch-btn-block-active')),
            'title' => t('Block view'),
          )
        )) .
        l(t('List view'), 'search/list/site/' . $adapter->getSearchKeys(), array(
          'query' => $adapter->getParams(),
          'attributes' => array(
          	'class' => array('search-display-switch-btn', 'search-display-switch-btn-list', ($display_type == 'list' ? 'search-display-switch-btn-list-active' : '')),
            'title' => t('List view'),
          )
        ))
    );
  }

  if(!$show_tabs) unset($variables['tabs']);
}

function beeldgeluid_page_alter(&$page) {
  if (!empty($page['content']['system_main']['search_form'])) {
    unset($page['content']['system_main']['search_form']);
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
function beeldgeluid_preprocess_node(&$variables, $hook) {
  $variables['classes_array'][] = 'view-mode-' . $variables['view_mode'];
  $variables['classes_array'][] = 'node-' . $variables['type'] . '-view-mode-' . $variables['view_mode'];

  // Optionally, run node-type-specific preprocess functions, like
  // beeldgeluid_preprocess_node_page() or beeldgeluid_preprocess_node_story().
  $function = __FUNCTION__ . '_' . $variables['node']->type;
  if (function_exists($function)) {
    $function($variables, $hook);
  }

  if ($variables['display_submitted'] == TRUE && $variables['node']->type == 'blog') {
    $variables['submitted'] = t('Submitted by !username on !datetime', array('!username' => $variables['name'], '!datetime' => format_date($variables['node']->created, 'short')));
  }

  if ($variables['node']->type == 'blog' && $variables['view_mode'] == 'full' && isset($variables['content']['links']['comment'])) {
    unset($variables['content']['links']['comment']);
  }
}

function beeldgeluid_preprocess_node_media(&$variables) {
  $titlebar_info = array();
  if (isset($variables['content']['group_info']['field_media_episode'][0]['#markup'])) {
    $titlebar_info[] = t('Episode') . ' ' . $variables['content']['group_info']['field_media_episode'][0]['#markup'];
  }
  if (isset($variables['content']['group_info']['field_media_date'][0]['#markup'])) {
    $titlebar_info[] = $variables['content']['group_info']['field_media_date'][0]['#markup'];
  }
  $variables['titlebar_info'] = implode (' &ndash; ', $titlebar_info);
}

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
 * Override or insert variables into the search result templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("search_results" in this case.)
 */
function beeldgeluid_preprocess_search_results(&$variables, $hook) {
  //dsm($variables);
}

/**
 * Override or insert variables into the search result templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 */
function beeldgeluid_preprocess_search_result(&$variables) {
  $display_type = arg(1) == 'list' ? arg(1) : 'block';
  $id = isset($_GET['page']) ? $_GET['page'] * 18 + $variables['id'] : $variables['id'];
  $style = _beeldgeluid_get_style($id, $display_type);
  $variables['classes_array'][] = 'search-result-' . $style;
  $variables['classes_array'][] = 'search-result-' . $id;

  if($display_type != 'list' && $style != 'grid-3x3') {
    unset($variables['snippet']);
  }

  if($variables['result']['entity_type'] == 'node') {
    // Load node
    $node = array_shift(entity_load('node', array($variables['result']['node']->entity_id)));
    if(isset($node->type)){
      switch($node->type) {
        // Entity types which have no image data attached
        case 'agenda':
        case 'article':
        case 'flash':
        case 'iframe':
        case 'news':
        case 'webform':
        case 'blog':
        case 'dossier':
          if(isset($node->field_image[$node->language][0]['uri'])) {
            $variables['image'] = array(
              '#theme'       => 'image_style',
              '#style_name'  => $style,
              '#path'        => $node->field_image[$node->language][0]['uri'],
            );
          }
          break;

        case 'media':
          if(isset($node->field_media_file[$node->language][0]['fid'])){
            $file = file_load($node->field_media_file[$node->language][0]['fid']);
            $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);

            if($wrapper instanceof BGMediaStreamWrapper) {
              $image_uri = $wrapper->getLocalThumbnailPath();
            }
            else if($wrapper instanceof DrupalPublicStreamWrapper && $file->type == 'image') {
              $image_uri = $wrapper->getUri();
            }

            if(isset($image_uri)) {
              $variables['image'] = array(
                '#theme'       => 'image_style',
                '#style_name'  => $style,
                '#path'        => $image_uri,
              );
            }
          }
          break;
      }
    }
  }
}

function _beeldgeluid_get_style($id, $display_type) {
  // List
  if ($display_type == 'list') {
    return 'grid-1x1';
  }
  // Large
  else if ($display_type != 'list'  && $id <= 4) {
    return 'grid-3x3';
  }
  // Medium
  else if ($display_type != 'list' && $id <= 10) {
    return 'grid-2x2';
  }
  // Small
  else {
    return 'grid-1_5x1_5';
  }
}

/**
 * Theme the way an 'all day' label will look.
 * In this theme, we return an empty string to disable the '(all day)' suffix.
 */
function beeldgeluid_date_all_day_label() {
  return '';
}

/**
 * Fix images sizes for external images.
 * See http://api.drupal.org/api/drupal/modules--image--image.module/function/theme_image_style/7#comment-14839
 */
function beeldgeluid_image_style($variables) {
  $style_name = $variables['style_name'];
  $path = $variables['path'];

  // theme_image() can only honor the $getsize parameter with local file paths.
  // The derivative image is not created until it has been requested so the file
  // may not yet exist, in this case we just fallback to the URL.
  $style_path = image_style_path($style_name, $path);
  if (!file_exists($style_path)) {
    $style_path = image_style_url($style_name, $path);
  }
  $variables['path'] = $style_path;

  if (is_file($style_path)) {
    if (list($width, $height, $type, $attributes) = @getimagesize($style_path)) {
      $variables['width'] = $width;
      $variables['height'] = $height;
    }
  }

  return theme('image', $variables);
}
