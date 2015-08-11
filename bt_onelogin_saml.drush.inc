<?php
/**
 * @file
 * Drush commands for bt_onelogin.
 */

define('BT_ONELOGIN_SAML_TOOLKIT_URI', 'https://github.com/onelogin/php-saml/archive/v2.6.0.zip');

/**
 * Implements hook_drush_command().
 */
function bt_onelogin_saml_drush_command() {
  $items = array();
  $items['php-saml-download'] = array(
    'description' => dt('Download Onelogin\'s php-saml toolkit.'),
    'aliases' => array('saml-download'),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function bt_onelogin_saml_drush_help($section) {
  switch ($section) {
    case 'drush:saml-download':
      return dt('Download and install the Onelogin php-saml toolkit from https://github.com/onelogin/php-saml/releases/tag/v2.6.0 to sites/all/libraries.');
  }
}

/**
 * Command to download the Chosen plugin.
 */
function drush_bt_onelogin_saml_php_saml_download() {
  $library_path = 'sites/all/libraries';
  $destination_path = 'php-saml';

  // Create the path if it does not exist.
  if (!is_dir($library_path)) {
    drush_op('mkdir', $library_path);
    drush_log(dt('Directory @path was created', array('@path' => $library_path)), 'notice');
  }

  // Save current working directory.
  $current_directory = getcwd();
  // Set the directory to the download location.
  chdir($library_path);

  // Download the zip archive.
  if ($filepath = drush_download_file(BT_ONELOGIN_SAML_TOOLKIT_URI)) {
    // Assumes filename is version number starting with v (v2.6.0.zip).
    $php_saml_path = 'php-saml-' . ltrim(basename($filepath, '.zip'), 'v');

    // Remove existing php-saml toolkit directory.
    if (is_dir($php_saml_path)) {
      drush_delete_dir($php_saml_path, TRUE);
      drush_log(dt('A existing php-saml toolkit was deleted from @path', array('@path' => $php_saml_path)), 'notice');
    }
    if (is_dir($destination_path)) {
      drush_delete_dir($destination_path, TRUE);
      drush_log(dt('A existing php-saml toolkit was deleted from @path', array('@path' => $destination_path)), 'notice');
    }

    // Decompress zip.
    drush_tarball_extract(basename($filepath));

    // Rename php-saml-{version} to php-saml.
    drush_move_dir($php_saml_path, $destination_path, TRUE);

    if (is_dir($destination_path)) {
      drush_log(dt('Onelogin php-saml toolkit has been installed in @path', array('@path' => $php_saml_path)), 'success');
    }
    else {
      drush_log(dt('Drush was unable to install the Onelogin php-saml toolkit to @path', array('@path' => $php_saml_path)), 'error');
    }
  }
  // Return to current working directory.
  chdir($current_directory);
}
