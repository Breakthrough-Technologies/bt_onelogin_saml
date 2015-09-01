<?php
/**
 * @file
 * Administrative functionality for bt_onelogin_saml.
 */

/**
 * Admin config form.
 *
 * @param array $form
 *   Drupal form array.
 * @param array $form_state
 *   Drupal form_state array.
 *
 * @return array
 *   Form render array.
 */
function saml_toolkit_settings($form, &$form_state) {
  global $base_url;
  $form['sp'] = array(
    '#type' => 'fieldset',
    '#title' => t('Service Provider'),
  );

  $form['sp']['metadata']['bt_onelogin_saml_anon_metadata'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow anonymous access to SAML Metadata XML.'),
    '#default_value' => variable_get('bt_onelogin_saml_anon_metadata', FALSE),
  );

  $metadata = bt_onelogin_saml_get_metadata();
  $form['sp']['metadata'][] = array(
    '#type' => 'fieldset',
    '#access' => $metadata != '',
    '#title' => t('View SAML Metadata XML'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    'metadata' => array(
      '#type' => 'textarea',
      '#title' => t('Metadata'),
      '#disabled' => 'disabled',
      '#default_value' => $metadata,
      '#suffix' => l(t('Open SAML Metadata XML in new window'),
        'onelogin_saml/metadata', array(
          'attributes' => array(
            'target' => '_blank',
          ),
        )),
    ),
  );

  $form['sp']['bt_onelogin_saml_sp_entityid'] = array(
    '#type' => 'textfield',
    '#title' => t('Entity id'),
    '#description' => t('Identifier of the SP entity (must be a URI)'),
    '#default_value' => variable_get('bt_onelogin_saml_sp_entityid', $base_url),
  );
  $form['sp']['bt_onelogin_saml_sp_acs_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Assertion consumer service url'),
    '#description' => t('URL endpoint where the <AuthnResponse> from the IdP will be returned. OneLogin Toolkit supports this endpoint for the HTTP-POST binding only.'),
    '#default_value' => variable_get('bt_onelogin_saml_sp_acs_url', ''),
  );
  $form['sp']['bt_onelogin_saml_sp_sls_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Single logout service url'),
    '#description' => t('URL endpoint where the <Logout Response> from the IdP will be returned. OneLogin Toolkit supports the HTTP-Redirect binding only for this endpoint'),
    '#default_value' => variable_get('bt_onelogin_saml_sp_sls_url', ''),
  );
  $form['sp']['bt_onelogin_saml_sp_nameidformat'] = array(
    '#type' => 'textfield',
    '#title' => t('Name id format'),
    '#description' => t('Specifies the constraints on the name identifier to be used to represent the requested subject.'),
    '#default_value' => variable_get('bt_onelogin_saml_sp_nameidformat', 'urn:oasis:names:tc:SAML:1.1:nameid-format:emailAddress'),
  );

  $form['sp']['bt_onelogin_saml_sp_mail_attribute'] = array(
    '#type' => 'textfield',
    '#title' => t('Mail attribute'),
    '#description' => t('Input name of SP mail attribute to authenticate user by email. Leave blank to use name id if nameidformat is urn:oasis:names:tc:SAML:1.1:nameid-format:emailAddress.'),
    '#default_value' => variable_get('bt_onelogin_saml_sp_mail_attribute', ''),
  );

  $form['sp']['bt_onelogin_saml_sp_x509cert'] = array(
    '#type' => 'textarea',
    '#title' => 'x509cert',
    '#description' => t('Usually x509cert of the SP are provided by files placed at the certs folder. But we can also provide them here.'),
    '#default_value' => variable_get('bt_onelogin_saml_sp_x509cert', ''),
  );

  $form['idp'] = array(
    '#type' => 'fieldset',
    '#title' => t('Identity Provider'),
  );

  $form['idp']['bt_onelogin_saml_idp_entityid'] = array(
    '#type' => 'textfield',
    '#title' => t('Entity id'),
    '#description' => t('Identifier of the IdP entity  (must be a URI)'),
    '#default_value' => variable_get('bt_onelogin_saml_idp_entityid', ''),
  );
  $form['idp']['bt_onelogin_saml_idp_sso_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Single sign on service url'),
    '#description' => t('URL Target of the IdP where the Authentication Request Message will be sent. OneLogin Toolkit supports the HTTP-Redirect binding only for this endpoint.'),
    '#default_value' => variable_get('bt_onelogin_saml_idp_sso_url', ''),
  );
  $form['idp']['bt_onelogin_saml_idp_sls_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Single logout service url'),
    '#description' => t('SLO endpoint where the SLO request info will be sent. OneLogin Toolkit supports the HTTP-Redirect binding only for this endpoint.'),
    '#default_value' => variable_get('bt_onelogin_saml_idp_sls_url', ''),
  );
  $form['idp']['bt_onelogin_saml_idp_x509cert'] = array(
    '#type' => 'textarea',
    '#title' => 'x509cert',
    '#description' => t('Public x509 certificate of the IdP.'),
    '#default_value' => variable_get('bt_onelogin_saml_idp_x509cert', ''),
  );

  $form['provisioning'] = array(
    '#type' => 'fieldset',
    '#title' => 'Drupal user provisioning',
  );
  $form['provisioning']['bt_onelogin_saml_autoprovision'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create user if they exist on IdP but not Drupal.'),
    '#default_value' => variable_get('bt_onelogin_saml_autoprovision', FALSE),
  );

  return system_settings_form($form);
}
