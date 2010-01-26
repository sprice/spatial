<?php

/**
 * Implementation of hook_feeds_plugins().
 *
 * Declare to Feeds that we have a parser plugin that it can use.
 */
function spatial_import_feeds_plugins() {
  $path = drupal_get_path('module', 'spatial_import');
  
  $info = array();
  
  $info['SpatialImportShapefileParser'] = array(
    'name' => 'Shapefile parser',
    'description' => 'Parse data in zipped shapefile format.',
    'handler' => array(
      'parent' => 'FeedsParser',
      'class' => 'SpatialImportShapefileParser',
      'file' => 'spatial_import.feeds.ShapefileParser.inc',
      'path' => $path,
    ),
  );
  
  return $info;
}

function spatial_import_feeds_node_processor_targets_alter(&$targets, $content_type) {
  $info = content_types($content_type);
  $fields = array();
  if (isset($info['fields']) && count($info['fields'])) {
    foreach ($info['fields'] as $field_name => $field) {
      if (in_array($field['type'], array('openlayers_wkt','geo'))) {
        $fields[$field['type'].':'.$field_name] = isset($field['widget']['label']) ? $field['widget']['label'] : $field_name;
      }
    }
  }
  
  foreach ($fields as $k => $name) {
    $targets[$k] = array(
      'name' => $name,
      'callback' => 'spatial_import_feeds_set_target',
      'description' => t('The CCK !name field of the node.', array('!name' => $name)),
    );
  }
}

/**
 * Callback for mapping. Here is where the actual mapping happens.
 *
 */
function spatial_import_feeds_set_target(&$node, $target, $value) {
  
  // Parse out the type and name of the field.
  $target_data = split(":",$target);
  $type = $target_data[0];
  $field_name = $target_data[1];
  
  // Load the field from the node
  $field = isset($node->$field_name) ? $node->$field_name : array();
  
  // openlayers_wkt is stored in field[0]['openlayers_wkt'],
  // geo is stored in field[0]['geo'] - same as type names
  $field_key = $type;
  
  // Handle multiple value fields.
  if (is_array($value)) {
    $i = 0;
    foreach ($value as $v) {
      if (!is_array($v) && !is_object($v)) {
        $field[$i][$field_key] = $v;
      }
      $i++;
    }
  }
  else {
    $field[0][$field_key] = $value;
  }

  $node->$field_name = $field;
}