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
  
  $pattern = '/^\/?(levenmetoranje|oranjebioscoop|inhuldiging)/';
  
  if (preg_match($pattern, current_path()) || preg_match($pattern, $_SERVER['REDIRECT_URL'])) {
    $path = drupal_get_path('theme', 'beeldgeluid') . '/js/tb-youtube.js';
    drupal_add_js($path, 'file');
  }
}

/**
 * Hook to detect if youtube video is loaded by media_youtube module
 * @todo: put this code at a place that better identifies youtube at this page
 * 
 * @param type $variables
 */
function beeldengeluid_preprocess_media_youtube_video(&$variables) {
  // but... it is not getting here...
  // then put it in the beeldgeluid_preprocess_html function.
  drupal_add_js(drupal_get_path('theme', 'beeldgeluid') . '/js/tb-youtube.js');
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
    array_unshift($variables['theme_hook_suggestions'], 'page__'.$variables['node']->type);

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
          'query' => $adapter->getUrlProcessor()->getParams(),
          'attributes' => array(
          	'class' => array('search-display-switch-btn', 'search-display-switch-btn-block', ($display_type == 'list' ? '' : 'search-display-switch-btn-block-active')),
            'title' => t('Block view'),
          )
        )) .
        l(t('List view'), 'search/list/site/' . $adapter->getSearchKeys(), array(
          'query' => $adapter->getUrlProcessor()->getParams(),
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
  if ($variables['node']->type == 'article') {
    //Construct the view
    $view_array = explode('-', $variables['node']->field_artikel_view['und'][0]['value']);
    //Load the view
    $view = views_get_view($view_array[0]);
    if ($view) {
      //Only execute if we have a view
      $view_result = $view->execute_display($view_array[1]);
      $variables['content']['field_artikel_view'][0]['#markup'] = $view_result['content'];
    }
    else {
      $variables['content']['field_artikel_view'][0]['#markup'] = '';
    }
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
        case 'blog':
        case 'dossier':
        case 'flash':
        case 'iframe':
        case 'news':
        case 'webform':
          $field_image = field_get_items('node', $node, 'field_image', $node->language);
          $field_meta_image = field_get_items('node', $node, 'field_meta_image', $node->language);

          // Check if image field is set
          if(isset($field_image[0]['uri'])) {
            $image_uri = $field_image[0]['uri'];
          }
          // Check if meta image field is set
          else if(isset($field_meta_image[0]['fid']) && is_numeric($field_meta_image[0]['fid'])) {
            $file = file_load($field_meta_image[0]['fid']);
            $preview = media_get_thumbnail_preview($file);
            $image_uri = $preview['#path'];
          }
          break;

        case 'media':
          $field_media_file = field_get_items('node', $node, 'field_media_file', $node->language);

          if(isset($field_media_file[0]['fid']) && is_numeric($field_media_file[0]['fid'])){
            $file = file_load($field_media_file[0]['fid']);
            $preview = media_get_thumbnail_preview($file);
            $image_uri = $preview['#path'];
          }
          break;
      }

      if(isset($image_uri) && !empty($image_uri)) {
        $variables['image'] = array(
          '#theme'       => 'image_style',
          '#style_name'  => $style,
          '#path'        => $image_uri,
        );
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

/**
 * Helper function that builds the nested lists of a Nice menu.
 *
 * @param $menu
 *   Menu array from which to build the nested lists.
 * @param $depth
 *   The number of children levels to display. Use -1 to display all children
 *   and use 0 to display no children.
 * @param $trail
 *   An array of parent menu items.
 *
 * Override of theme_nice_menus_build(). Adds link classes to <li> as well.
 */
function beeldgeluid_nice_menus_build($variables) {
  $menu = $variables['menu'];
  $depth = $variables['depth'];
  $trail = $variables['trail'];
  $output = '';
  // Prepare to count the links so we can mark first, last, odd and even.
  $index = 0;
  $count = 0;
  foreach ($menu as $menu_count) {
    if ($menu_count['link']['hidden'] == 0) {
      $count++;
    }
  }
  // Get to building the menu.
  foreach ($menu as $menu_item) {
    $mlid = $menu_item['link']['mlid'];
    // Check to see if it is a visible menu item.
    if (!isset($menu_item['link']['hidden']) || $menu_item['link']['hidden'] == 0) {
      // Check our count and build first, last, odd/even classes.
      $index++;
      $first_class = $index == 1 ? ' first ' : '';
      $oddeven_class = $index % 2 == 0 ? ' even ' : ' odd ';
      $last_class = $index == $count ? ' last ' : '';
      // Build class name based on menu path
      // e.g. to give each menu item individual style.
      // Strip funny symbols.
      $clean_path = str_replace(array('http://', 'www', '<', '>', '&', '=', '?', ':', '.'), '', $menu_item['link']['href']);
      // Convert slashes to dashes.
      $clean_path = str_replace('/', '-', $clean_path);
      $class = 'menu-path-' . $clean_path;
      if ($trail && in_array($mlid, $trail)) {
        $class .= ' active-trail';
      }
      if (isset($menu_item['link']['localized_options']['attributes']['class'])) {
        $class .= ' ' . implode(' ', $menu_item['link']['localized_options']['attributes']['class']);
      }
      // If it has children build a nice little tree under it.
      if ((!empty($menu_item['link']['has_children'])) && (!empty($menu_item['below'])) && $depth != 0) {
        // Keep passing children into the function 'til we get them all.
        $children = theme('nice_menus_build', array('menu' => $menu_item['below'], 'depth' => $depth, 'trail' => $trail));
        // Set the class to parent only of children are displayed.
        $parent_class = ($children && ($menu_item['link']['depth'] <= $depth || $depth == -1)) ? 'menuparent ' : '';

        $element = array(
            '#below' => '',
            '#title' => $menu_item['link']['link_title'],
            '#href' =>  $menu_item['link']['href'],
            '#localized_options' => $menu_item['link']['localized_options'],
            '#attributes' => array(),
        );
        $variables['element'] = $element;

        $output .= '<li class="menu-' . $mlid . ' ' . $parent_class . $class . $first_class . $oddeven_class . $last_class . '">'. theme('nice_menus_menu_item_link', $variables);
        // Check our depth parameters.
        if ($menu_item['link']['depth'] <= $depth || $depth == -1) {
          // Build the child UL only if children are displayed for the user.
          if ($children) {
            $output .= '<ul>';
            $output .= $children;
            $output .= "</ul>\n";
          }
        }
        $output .= "</li>\n";
      }
      else {

        $element = array(
            '#below' => '',
            '#title' => $menu_item['link']['link_title'],
            '#href' =>  $menu_item['link']['href'],
            '#localized_options' => $menu_item['link']['localized_options'],
            '#attributes' => array(),
        );
        $variables['element'] = $element;
        $output .= '<li class="menu-' . $mlid . ' ' . $class . $first_class . $oddeven_class . $last_class . '">' . theme('nice_menus_menu_item_link', $variables) . "</li>\n";
      }
    }
  }
  return $output;
}
