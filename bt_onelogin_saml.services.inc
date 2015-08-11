<?php
/**
 * @file
 * SSO, SLO, and ACS services for the onelogin php-saml toolkit.
 */

/**
 * Page callback for assertion consumer services.
 *
 * @return string|void
 *   String if user was not able to be authenticated. Void otherwise.
 *
 * @throws OneLogin_Saml2_Error
 */
function saml_toolkit_acs() {
  $auth = bt_onelogin_saml_get_instance();
  $auth->processResponse();

  $errors = $auth->getErrors();

  if (!empty($errors)) {
    foreach ($errors as $error) {
      drupal_set_message($error, 'error');
    }
  }

  if (!$auth->isAuthenticated()) {
    return "<p>Not authenticated</p>";
  }

  $name_id = $auth->getNameId();
  $user_data = $auth->getAttributes();
  $mail = !empty(variable_get('bt_onelogin_saml_sp_mail_attribute')) ?
    variable_get('bt_onelogin_saml_sp_mail_attribute') : $name_id;

  // Does the user exist?
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'user')
    ->propertyCondition('status', 1)
    ->propertyCondition('mail', $mail);

  // If a user exists, attempt to authenticate.
  $result = $query->execute();
  if ($result && array_key_exists('user', $result)) {
    $user = user_load(key($result['user']));
    saml_authenticate($user);
  }

  // Other create user is autoprovision setting enabled.
  elseif (variable_get('bt_onelogin_saml_autoprovision', FALSE)) {
    $user = saml_provision($mail);
    saml_authenticate($user);
  }

  $_SESSION['samlUserdata'] = $user_data;
  $_SESSION['samlNameId'] = $name_id;
  $_SESSION['samlSessionIndex'] = $auth->getSessionIndex();
  if (isset($_POST['RelayState']) && OneLogin_Saml2_Utils::getSelfURL() != $_POST['RelayState']) {
    drupal_goto($_POST['RelayState']);
  }
}

/**
 * Authenticate a user who logged in via SSO.
 *
 * @param object $user
 *   Drupal user object.
 */
function saml_authenticate($user) {
  $GLOBALS['user'] = $user;
  $edit['uid'] = $user->uid;
  user_login_finalize($edit);
}

/**
 * Creates user if they do not exist in Drupal.
 *
 * @param string $mail
 *   Email address of the user.
 *
 * @return object
 *   Drupal user object.
 * @throws Exception
 */
function saml_provision($mail) {
  // Create the user entity. This module requires email_registration which
  // will find the email_registration_ prefix during the user creation hooks
  // and create a unique username based on email.
  $name = 'email_registration_' . user_password();
  $data = array(
    'name' => $name,
    'mail' => $mail,
    'init' => $mail,
    'pass' => user_password(),
    'status' => TRUE,
  );
  $data['roles'][DRUPAL_AUTHENTICATED_RID] = 'authenticated user';

  module_invoke_all('bt_onelogin_saml_pre_provision_user');
  $account = entity_create('user', array());
  user_save($account, $data);
  module_invoke_all('bt_onelogin_saml_post_provision_user');
  return $account;
}

/**
 * Page callback for single logout service.
 *
 * Logs out of SSO, then redirects to the SP settings for the single logout
 * service url (usually user/logout).
 *
 * @throws OneLogin_Saml2_Error
 */
function saml_toolkit_sls() {
  // Kill SSO session.
  $auth = bt_onelogin_saml_get_instance();
  $return_to = NULL;
  $parameters = array();
  $name_id = isset($_SESSION['samlNameId']) ? $_SESSION['samlNameId'] : NULL;
  $session_index = isset($_SESSION['samlSessionIndex']) ?
    $_SESSION['samlSessionIndex'] : NULL;
  $auth->logout($return_to, $parameters, $name_id, $session_index);

}

/**
 * Page callback for single sign on.
 *
 * Logs user into SSO, then redirects to the desired location.
 */
function saml_toolkit_sso() {
  $auth = bt_onelogin_saml_get_instance();
  $return_to = NULL;
  $destination = drupal_get_destination();
  if (array_key_exists('destination', $destination)) {
    $destination = $destination['destination'];
    if ($destination != current_path()) {
      $return_to = $destination;
    }
  }
  $auth->login($return_to);
}
