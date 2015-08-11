<?php
/**
 * @file
 * Page functionality for the bt_onelogin_saml module.
 */

/**
 * Page callback to retrieve SAML Metadata XML.
 *
 * @return array
 *   An empty array if an error occurs, otherwise XML content.
 */
function saml_toolkit_metadata() {
  $errors = array();
  $xml = bt_onelogin_saml_get_metadata($errors);
  if (empty($errors)) {
    drupal_add_http_header('Content-Length', strlen($xml));
    drupal_add_http_header('Content-Type', 'text/xml');
    echo $xml;
    drupal_exit();
  }
  foreach ($errors as $error) {
    drupal_set_message($error, 'error');
  }
  return array();
}
