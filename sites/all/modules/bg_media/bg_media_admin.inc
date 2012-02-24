<?php

/**
 * Settings for the default formatter.
 */
function bg_media_default_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $formatter = $display['type'];
  $form = array();

  $form['format_type_year'] = array(
    '#title' => t('Choose how users view dates with accuracy of a year:'),
    '#type' => 'select',
    '#options' => date_format_type_options(),
    '#default_value' => $settings['format_type_year'],
    '#description' => t('To add or edit options, visit <a href="@date-time-page">Date and time settings</a>.', array('@date-time-page' => url('admin/config/regional/date-time'))),
    '#weight' => 0,
  );
  $form['format_type_month'] = array(
    '#title' => t('Choose how users view dates with accuracy of a month:'),
    '#type' => 'select',
    '#options' => date_format_type_options(),
    '#default_value' => $settings['format_type_month'],
    '#description' => t('To add or edit options, visit <a href="@date-time-page">Date and time settings</a>.', array('@date-time-page' => url('admin/config/regional/date-time'))),
    '#weight' => 0,
  );
  $form['format_type_day'] = array(
    '#title' => t('Choose how users view dates with accuracy of a day:'),
    '#type' => 'select',
    '#options' => date_format_type_options(),
    '#default_value' => $settings['format_type_day'],
    '#description' => t('To add or edit options, visit <a href="@date-time-page">Date and time settings</a>.', array('@date-time-page' => url('admin/config/regional/date-time'))),
    '#weight' => 0,
  );
  return $form;
}
