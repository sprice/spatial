<?php

/**
 * @file
 * A feeds plugin.
 */

/**
 * Implements of hook_feeds_plugins().
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