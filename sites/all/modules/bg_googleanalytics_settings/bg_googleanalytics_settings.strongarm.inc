<?php
/**
 * @file
 * bg_googleanalytics_settings.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function bg_googleanalytics_settings_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'googleanalytics_codesnippet_after';
  $strongarm->value = '// extra code
jQuery(function() {
  jQuery(\'a[href^="mailto"]\').click(function() {
    var email = jQuery(this).attr(\'href\').replace(\'mailto:\', \'\');
    _gaq.push([\'_trackEvent\', \'External\', \'E-mail\', email]);
  });
});
jQuery.expr[\':\'].external = function(obj) {
  return !obj.href.match(/^mailto:/) && !obj.href.match(/^javascript:/) &&
          (obj.hostname != document.location.hostname);
};
jQuery(function() {
  jQuery(\'a:external\').click(function() {
    _gaq.push([\'_trackEvent\', \'External\', \'Link\', jQuery(this).attr(\'href\')]);
  });
});
jQuery(function() {
  var extensions = ["avi", "css", "doc", "eps", "exe", "jpg", "js", "mov", "mp3",
    "pdf", "png", "ppt", "rar", "svg", "txt", "vsd", "vxd", "wma", "wmv", "xls", "zip"];
  var extensionsPattern = new RegExp(\'\\\\.((\' + extensions.join(\')|(\') + \'))$\',
          \'g\');
  jQuery(\'a\').filter(function() {
    return this.href.match(extensionsPattern);
  }).click(function() {
    _gaq.push([\'_trackEvent\', \'File\', \'Download\', jQuery(this).attr(\'href\')]);
  });
});';
  $export['googleanalytics_codesnippet_after'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'googleanalytics_codesnippet_before';
  $strongarm->value = 'var pluginUrl = \'//www.google-analytics.com/plugins/ga/inpage_linkid.js\';_gaq.push([\'_require\', \'inpage_linkid\', pluginUrl]);';
  $export['googleanalytics_codesnippet_before'] = $strongarm;

  return $export;
}
