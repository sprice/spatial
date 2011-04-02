<?php

/**
 * @file
 * ShapefileParser class.
 */

/**
 * Class definition for Shapefile Parser.
 *
 * Parses a given file as a zipped shapefile.
 */
class SpatialImportShapefileParser extends FeedsParser {

  /**
   * Implements FeedsParser::parse().
   */
  public function parse(FeedsSource $source, FeedsFetcherResult $fetcher_result) {
    $source_config = $source->getConfigFor($this);
    $state = $source->state(FEEDS_PARSE);
    $result = new FeedsParserResult();
    // @TODO don't hardcode this!
    $shapefile = file_load('1');
    $shapefile_data = spatial_import_process_shapefile($shapefile, $source_config['spatial_field_name'], 'wkt');
    foreach ($shapefile_data['columns'] as $i => $title) {
      $shapefile_data['columns'][$i] = strtolower($title); // Use lower case only.
    }
    foreach ($shapefile_data['data'] as $i => $row) {
      $item = array();
      foreach ($row as $j => $col) {
        $item[$shapefile_data['columns'][$j]] = $col;
      }
      $result->items[] = $item;
    }
    return $result;
    
    
    /*
     $options = $this->config;
    // Apply titles in lower case.
    foreach ($shapefile_data['columns'] as $i => $title) {
      
      $shapefile_data['columns'][$i] = strtolower($title); // Use lower case only.
    }
    $result_rows = array();
    foreach ($shapefile_data['data'] as $i => $row) {
      $result_row = array();
      foreach ($row as $j => $col) {
        $result_row[$shapefile_data['columns'][$j]] = $col;
      }
      $result_rows[] = $result_row;
    }
    
    // Return result.
    $batch->setItems($result_rows);
    return $result;
    */
  }

  /**
   * Override parent::getSourceElement to use only lower keys.
   */
  public function getSourceElement($item, $element_key) {
    $element_key = strtolower($element_key);
    return isset($item[$element_key]) ? $item[$element_key] : '';
  }

  /**
   * Define defaults.
   */
  public function sourceDefaults() {
    return array();
  }

  /**
   * Source form.
   *
   * Show mapping configuration as a guidance for import form users.
   */
  
  public function sourceForm($source_config) {
    $form = array();
    $form['#weight'] = -10;

    $mappings = feeds_importer($this->id)->processor->config['mappings'];
    $sources = $uniques = array();
    foreach ($mappings as $mapping) {
      $sources[] = $mapping['source'];
      if ($mapping['unique']) {
        $uniques[] = $mapping['source'];
      }
    }

    $items = array(
      t('Import zipped !shapefile with one or more of these columns: !columns.', array('!shapefile' => l(t('Shapefiles'), 'http://en.wikipedia.org/wiki/Shapefile'), '!columns' => implode(', ', $sources))),
      format_plural(count($uniques), t('Column <strong>!column</strong> is mandatory and considered unique: only one item per !column value will be created.', array('!column' => implode(', ', $uniques))), t('Columns <strong>!columns</strong> are mandatory and values in these columns are considered unique: only one entry per value in one of these column will be created.', array('!columns' => implode(', ', $uniques)))),
    );
    $form['help']['#value'] = '<div class="help">' . theme('item_list', $items) . '</div>';
    
    return $form;
  }
  
  /**
   * Define default configuration.
   */

  public function configDefaults() {
    return array(
      'spatial_field_name' => 'geom',
      'srid' => '4326',
      'storage' => 'wkt',
    );
  }
  
  /**
   * Build configuration form.
   */

  public function configForm(&$form_state) {
    $form = array();
    $form['spatial_field_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Spatial Data Column'),
      '#default_value' => $this->config['spatial_field_name'],
      '#size' => 24,
      '#maxlength' => 24,
      '#description' => t('Shapefiles do not contain a geometry \'column\'. Please specify the a name for the column where the geometry will be stored. This column can then be referenced by \'Name of source field\' in field mappings.')
    );
    
    $form['srid'] = array(
      '#type' => 'textfield',
      '#title' => t('SRID (Spatial projection)'),
      '#default_value' => $this->config['srid'],
      '#size' => 24,
      '#maxlength' => 24,
      '#description' => t('Integer representing the SRID of the uploaded shapefile. If you are unsure, use \'4326\' (lat/lon).')
    );
    
    $form['storage'] = array(
      '#type' => 'hidden',
      '#title' => t('Database Storage'),
      '#value' => 'wkt',
      '#required' => TRUE,
    );
    return $form;
  }
  
}
