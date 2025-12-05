# View Pattern: ASCII Diagrams and Generic Examples

**Date**: December 5, 2025  
**Context**: Supplemental material with diagrams and generic examples  
**Related**: SAVE_LOAD_WORKFLOW_ANALYSIS.md, CONFIGURATION_VS_DATA_STRUCTS_ANALYSIS.md

---

## Table of Contents

1. [ASCII Diagrams](#ascii-diagrams)
2. [Generic Example (Non-Game)](#generic-example-non-game)
3. [Interfacing with Running Game](#interfacing-with-running-game)

---

## ASCII Diagrams

### Diagram 1: Hierarchical View Pattern Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                    HIERARCHICAL VIEW PATTERN                        │
│                                                                     │
│  Problem: Load nested data without copying or format coupling      │
│  Solution: Navigate DOWN with views, Extract UP with structs       │
└─────────────────────────────────────────────────────────────────────┘

┌───────────────────────────────────────────────────────────────────────┐
│                         DATA SOURCE LAYER                             │
│  (FlatBuffers binary file, JSON file, XML, Database, Network, etc.)  │
└───────────────────────────────────────────────────────────────────────┘
                                │
                                │ Provider loads data
                                ▼
┌───────────────────────────────────────────────────────────────────────┐
│                      PROVIDER LAYER (Format-Specific)                 │
│                                                                       │
│  FlatbuffersProvider          JsonProvider          XmlProvider      │
│       │                            │                     │            │
│       └────────────────────────────┴─────────────────────┘            │
│                                │                                      │
│                    Implements View Interfaces                         │
│                                │                                      │
│                                ▼                                      │
│         ┌──────────────────────────────────────┐                     │
│         │   View Implementation (Concrete)     │                     │
│         │   • FlatbuffersRootView              │                     │
│         │   • FlatbuffersContainerView         │                     │
│         │   • FlatbuffersItemView              │                     │
│         └──────────────────────────────────────┘                     │
└───────────────────────────────────────────────────────────────────────┘
                                │
                                │ Returns view interface
                                ▼
┌───────────────────────────────────────────────────────────────────────┐
│                    VIEW INTERFACE LAYER (Abstract)                    │
│                                                                       │
│    ┌─────────────────┐                                               │
│    │  IRootView      │  ← Root of hierarchy                          │
│    │  • GetConfig()  │    Returns: ConfigData (native struct)        │
│    │  • GetContainer()│  Returns: IContainerView (view interface)    │
│    └─────────────────┘                                               │
│            │                                                          │
│            │ Navigate down                                            │
│            ▼                                                          │
│    ┌─────────────────┐                                               │
│    │ IContainerView  │  ← Level 2 of hierarchy                       │
│    │ • GetMetadata() │    Returns: Metadata (native struct)          │
│    │ • GetItemCount()│    Returns: size_t (primitive)                │
│    │ • GetItem(i)    │    Returns: IItemView (view interface)        │
│    │ • ForEachItem() │    Callback: (IItemView&) → void              │
│    └─────────────────┘                                               │
│            │                                                          │
│            │ Navigate down                                            │
│            ▼                                                          │
│    ┌─────────────────┐                                               │
│    │  IItemView      │  ← Leaf level                                 │
│    │  • GetID()      │    Returns: string (native type)              │
│    │  • GetValue()   │    Returns: double (primitive)                │
│    │  • GetData()    │    Returns: ItemData (native struct)          │
│    └─────────────────┘                                               │
└───────────────────────────────────────────────────────────────────────┘
                                │
                                │ Game code uses view interfaces only
                                ▼
┌───────────────────────────────────────────────────────────────────────┐
│                        GAME CODE LAYER                                │
│                    (Format-Independent)                               │
│                                                                       │
│  void LoadData() {                                                    │
│    IProvider& provider = GetProvider();                               │
│    IRootView root = provider.LoadRootView();  ← View interface       │
│                                                                       │
│    // Extract simple data as native struct                            │
│    ConfigData config = root.GetConfig();      ← Native struct        │
│    ApplyConfig(config);                                               │
│                                                                       │
│    // Navigate down to container                                      │
│    IContainerView container = root.GetContainer(); ← View interface  │
│                                                                       │
│    // Iterate items                                                   │
│    container.ForEachItem([](IItemView& item) { ← View interface      │
│      // Extract data and configure                                    │
│      ItemData data = item.GetData();          ← Native struct        │
│      ConfigureItem(data);                                             │
│    });                                                                │
│  }                                                                    │
│                                                                       │
│  ✅ No format types (FlatBuffers, JSON, etc.)                        │
│  ✅ No intermediate copies                                            │
│  ✅ Direct access via views                                           │
└───────────────────────────────────────────────────────────────────────┘


KEY PRINCIPLES:
═══════════════

1. Navigate DOWN    → Use view interfaces (zero-copy)
2. Extract UP       → Native structs for simple data
3. Configure        → In-place from views
4. No Coupling      → Game code never sees format types
```

### Diagram 2: Data Flow During Load

```
┌─────────────────────────────────────────────────────────────────────┐
│                         LOAD DATA FLOW                              │
└─────────────────────────────────────────────────────────────────────┘

TIME →

Step 1: Provider Loads Data
───────────────────────────
    ┌──────────┐
    │ Provider │ ──reads──→ [File/Network/DB]
    └──────────┘
         │
         │ loads to memory
         ▼
    ┌──────────────────┐
    │ Binary Buffer    │  ← Provider owns this
    │ (FlatBuffers/    │     Stays in memory
    │  JSON parsed)    │     while views exist
    └──────────────────┘
         │
         │ wraps in view
         ▼
    ┌──────────────────┐
    │ Concrete View    │  ← References buffer
    │ Implementation   │     No copy!
    └──────────────────┘
         │
         │ returns interface
         ▼
    ┌──────────────────┐
    │ IView Interface  │  ← Returned to caller
    └──────────────────┘


Step 2: Game Code Navigates
────────────────────────────
    ┌──────────────────┐
    │ Game Code        │
    │ Receives IView   │
    └──────────────────┘
         │
         │ calls GetConfig()
         ▼
    ┌──────────────────┐        ┌─────────────────┐
    │ View reads from  │ ────→  │ Binary Buffer   │
    │ buffer           │        │ (still there)   │
    └──────────────────┘        └─────────────────┘
         │
         │ converts to native
         ▼
    ┌──────────────────┐
    │ ConfigData       │  ← Native struct created
    │ (native struct)  │     Copied from buffer
    └──────────────────┘
         │
         │ returned to game
         ▼
    ┌──────────────────┐
    │ Game Code        │  ← Uses native struct
    │ config.width     │     No format dependency!
    └──────────────────┘


Step 3: Navigate to Nested Data
────────────────────────────────
    ┌──────────────────┐
    │ Game Code        │
    │ view.GetContainer()
    └──────────────────┘
         │
         │ navigate down
         ▼
    ┌──────────────────┐        ┌─────────────────┐
    │ View wraps       │ ────→  │ Binary Buffer   │
    │ nested section   │        │ (still there)   │
    └──────────────────┘        └─────────────────┘
         │
         │ returns interface
         ▼
    ┌──────────────────┐
    │ IContainerView   │  ← New view to nested data
    │ Interface        │     Still no copying!
    └──────────────────┘
         │
         │ game continues
         ▼
    ┌──────────────────┐
    │ Game Code        │
    │ ForEachItem(...)│
    └──────────────────┘


Step 4: Configure Objects
──────────────────────────
    ┌──────────────────┐
    │ ForEachItem      │
    │ callback         │
    └──────────────────┘
         │
         │ for each item
         ▼
    ┌──────────────────┐        ┌─────────────────┐
    │ View wraps       │ ────→  │ Binary Buffer   │
    │ item data        │        │ (still there)   │
    └──────────────────┘        └─────────────────┘
         │
         │ extract native data
         ▼
    ┌──────────────────┐
    │ ItemData         │  ← Native struct
    │ (copied)         │
    └──────────────────┘
         │
         │ configure game object
         ▼
    ┌──────────────────┐
    │ Game Object      │  ← Configured in-place
    │ configured       │
    └──────────────────┘


Memory Timeline
───────────────

[Load Start]
    Provider loads      →  Buffer in memory
    Creates view        →  View references buffer
    Returns interface   →  Game receives interface
[Game Code]
    Navigate views      →  Views reference buffer
    Extract native      →  Small copies (primitives, structs)
    Configure objects   →  Objects configured
[Load End]
    Views destroyed     →  No more references
    Buffer deallocated  →  Memory freed

Key: Buffer stays in memory while ANY view references it
     Views are lightweight (just pointers/references)
     Only simple data copied (native structs)
```

### Diagram 3: Comparison with Other Approaches

```
┌─────────────────────────────────────────────────────────────────────┐
│              APPROACH COMPARISON: VIEW vs ALTERNATIVES              │
└─────────────────────────────────────────────────────────────────────┘

Approach 1: DIRECT FORMAT COUPLING (Current - Bad)
───────────────────────────────────────────────────

[File] → FlatBuffers → Game Code
                           │
                           └─→ Uses FlatBuffers types directly!
                               const FlatbufferData* data
                               if (data->field()) ...

Problems: ❌ Format coupling
          ❌ Can't swap formats
          ❌ Hard to test


Approach 2: INTERMEDIATE STRUCTS (Alternative - Bad)
─────────────────────────────────────────────────────

[File] → FlatBuffers → Convert → IntermediateStruct → Game Code
                           │            │
                           │            └─→ Huge memory overhead!
                           │                Recursive copying
                           │                Parallel hierarchy
                           └────────────────────────────┘
                                    Wasteful

Problems: ❌ Memory overhead
          ❌ Slow (deep copy)
          ❌ Maintenance burden


Approach 3: VIEW PATTERN (Proposed - Good)
───────────────────────────────────────────

[File] → FlatBuffers ──wraps──→ View ──interface──→ Game Code
             │                    │                      │
             │ (stays in memory)  │ (lightweight)        │ (format-free)
             │                    │                      │
             └────────references──┘                      │
                                                         │
                                  ┌──────────────────────┘
                                  │
                                  └─→ IView interface only
                                      No format types!
                                      Zero-copy navigation
                                      Direct access

Benefits: ✅ No coupling
          ✅ Zero-copy
          ✅ Format agnostic


Side-by-Side Memory Comparison
───────────────────────────────

Intermediate Struct Approach:
┌────────────────┐   ┌────────────────┐   ┌────────────────┐
│ FlatBuffers    │   │ Intermediate   │   │ Game Objects   │
│ Buffer         │ → │ Structs        │ → │ (configured)   │
│ 1 MB           │   │ 1 MB (copy!)   │   │ 0.5 MB         │
└────────────────┘   └────────────────┘   └────────────────┘
Total: 2.5 MB in memory at once!

View Pattern Approach:
┌────────────────┐                       ┌────────────────┐
│ FlatBuffers    │   ┌──────────────┐    │ Game Objects   │
│ Buffer         │ ← │ Views        │ →  │ (configured)   │
│ 1 MB           │   │ (pointers!)  │    │ 0.5 MB         │
└────────────────┘   └──────────────┘    └────────────────┘
Total: 1.5 MB in memory (40% less!)
```

---

## Generic Example (Non-Game)

### Scenario: Product Catalog System

Let's use a product catalog as a generic example - no game concepts.

**Data Structure**:
```
Catalog
  ├─ CatalogInfo (name, version, currency)
  └─ Categories[]
      ├─ CategoryInfo (id, name, description)
      └─ Products[]
          └─ ProductInfo (sku, name, price, stock)
```

### Step 1: Define View Interfaces (Format-Independent)

```cpp
//==============================================================================
// ViewInterfaces.h - No format dependencies!
//==============================================================================

#include <string>
#include <functional>

// Simple data structs (POD types)
struct CatalogInfo {
  std::string name;
  std::string version;
  std::string currency;
};

struct CategoryInfo {
  std::string id;
  std::string name;
  std::string description;
};

struct ProductInfo {
  std::string sku;
  std::string name;
  double price;
  int stock;
};

//------------------------------------------------------------------------------
// View Interfaces
//------------------------------------------------------------------------------

class IProductView {
public:
  virtual ~IProductView() = default;
  
  // Extract all data as native struct
  virtual ProductInfo GetProductInfo() const = 0;
  
  // Or access fields individually
  virtual std::string GetSKU() const = 0;
  virtual std::string GetName() const = 0;
  virtual double GetPrice() const = 0;
  virtual int GetStock() const = 0;
};

class ICategoryView {
public:
  virtual ~ICategoryView() = default;
  
  // Simple data
  virtual CategoryInfo GetCategoryInfo() const = 0;
  virtual std::string GetID() const = 0;
  virtual std::string GetName() const = 0;
  
  // Navigation to products
  virtual size_t GetProductCount() const = 0;
  virtual IProductView GetProduct(size_t index) const = 0;
  
  // Iteration helper
  virtual void ForEachProduct(
      std::function<void(const IProductView&)> callback) const = 0;
};

class ICatalogView {
public:
  virtual ~ICatalogView() = default;
  
  // Simple data
  virtual CatalogInfo GetCatalogInfo() const = 0;
  
  // Navigation to categories
  virtual size_t GetCategoryCount() const = 0;
  virtual ICategoryView GetCategory(size_t index) const = 0;
  virtual ICategoryView GetCategoryByID(const std::string& id) const = 0;
  
  // Iteration helper
  virtual void ForEachCategory(
      std::function<void(const ICategoryView&)> callback) const = 0;
};
```

### Step 2: Implement Provider Interface

```cpp
//==============================================================================
// ICatalogProvider.h - Provider interface (format-independent)
//==============================================================================

class ICatalogProvider {
public:
  virtual ~ICatalogProvider() = default;
  
  virtual ICatalogView LoadCatalog() = 0;
};
```

### Step 3: Implement JSON Provider (Format-Specific)

```cpp
//==============================================================================
// JsonCatalogProvider.h - JSON-specific implementation
//==============================================================================

#include "ICatalogProvider.h"
#include "ViewInterfaces.h"
#include <nlohmann/json.hpp>  // JSON library
#include <memory>

// Forward declarations
class JsonCatalogView;
class JsonCategoryView;
class JsonProductView;

//------------------------------------------------------------------------------
// JSON Provider
//------------------------------------------------------------------------------

class JsonCatalogProvider : public ICatalogProvider {
private:
  std::shared_ptr<nlohmann::json> m_json_data;  // Owns the JSON
  
public:
  ICatalogView LoadCatalog() override;
};

//------------------------------------------------------------------------------
// JSON View Implementations (Concrete)
//------------------------------------------------------------------------------

class JsonCatalogView : public ICatalogView {
private:
  std::shared_ptr<nlohmann::json> m_json;  // Shares ownership
  const nlohmann::json* m_catalog_node;     // Points to catalog node
  
public:
  JsonCatalogView(std::shared_ptr<nlohmann::json> json,
                  const nlohmann::json* node)
      : m_json(json), m_catalog_node(node) {}
  
  CatalogInfo GetCatalogInfo() const override {
    CatalogInfo info;
    info.name = (*m_catalog_node)["name"].get<std::string>();
    info.version = (*m_catalog_node)["version"].get<std::string>();
    info.currency = (*m_catalog_node)["currency"].get<std::string>();
    return info;
  }
  
  size_t GetCategoryCount() const override {
    return (*m_catalog_node)["categories"].size();
  }
  
  ICategoryView GetCategory(size_t index) const override {
    const auto& cat_node = (*m_catalog_node)["categories"][index];
    return JsonCategoryView(m_json, &cat_node);
  }
  
  void ForEachCategory(
      std::function<void(const ICategoryView&)> callback) const override {
    for (size_t i = 0; i < GetCategoryCount(); ++i) {
      auto cat_view = GetCategory(i);
      callback(cat_view);
    }
  }
  
  // ... other methods
};

class JsonCategoryView : public ICategoryView {
private:
  std::shared_ptr<nlohmann::json> m_json;
  const nlohmann::json* m_category_node;
  
public:
  JsonCategoryView(std::shared_ptr<nlohmann::json> json,
                   const nlohmann::json* node)
      : m_json(json), m_category_node(node) {}
  
  CategoryInfo GetCategoryInfo() const override {
    CategoryInfo info;
    info.id = (*m_category_node)["id"].get<std::string>();
    info.name = (*m_category_node)["name"].get<std::string>();
    info.description = (*m_category_node)["description"].get<std::string>();
    return info;
  }
  
  size_t GetProductCount() const override {
    return (*m_category_node)["products"].size();
  }
  
  IProductView GetProduct(size_t index) const override {
    const auto& prod_node = (*m_category_node)["products"][index];
    return JsonProductView(m_json, &prod_node);
  }
  
  void ForEachProduct(
      std::function<void(const IProductView&)> callback) const override {
    for (size_t i = 0; i < GetProductCount(); ++i) {
      auto prod_view = GetProduct(i);
      callback(prod_view);
    }
  }
  
  // ... other methods
};

class JsonProductView : public IProductView {
private:
  std::shared_ptr<nlohmann::json> m_json;
  const nlohmann::json* m_product_node;
  
public:
  JsonProductView(std::shared_ptr<nlohmann::json> json,
                  const nlohmann::json* node)
      : m_json(json), m_product_node(node) {}
  
  ProductInfo GetProductInfo() const override {
    ProductInfo info;
    info.sku = (*m_product_node)["sku"].get<std::string>();
    info.name = (*m_product_node)["name"].get<std::string>();
    info.price = (*m_product_node)["price"].get<double>();
    info.stock = (*m_product_node)["stock"].get<int>();
    return info;
  }
  
  std::string GetSKU() const override {
    return (*m_product_node)["sku"].get<std::string>();
  }
  
  // ... other methods
};
```

### Step 4: Application Code (Format-Independent!)

```cpp
//==============================================================================
// Application.cpp - Business logic (NO JSON dependency!)
//==============================================================================

#include "ViewInterfaces.h"
#include "ICatalogProvider.h"
#include <iostream>

class CatalogManager {
private:
  ICatalogProvider& m_provider;
  
public:
  CatalogManager(ICatalogProvider& provider) : m_provider(provider) {}
  
  void LoadAndDisplay() {
    // 1. Load catalog (returns view)
    ICatalogView catalog = m_provider.LoadCatalog();
    
    // 2. Extract simple data
    CatalogInfo info = catalog.GetCatalogInfo();
    std::cout << "Catalog: " << info.name << " v" << info.version << "\n";
    std::cout << "Currency: " << info.currency << "\n\n";
    
    // 3. Navigate and process
    catalog.ForEachCategory([](const ICategoryView& category) {
      // Extract category info
      CategoryInfo cat_info = category.GetCategoryInfo();
      std::cout << "Category: " << cat_info.name << "\n";
      std::cout << "  " << cat_info.description << "\n\n";
      
      // Process products
      category.ForEachProduct([](const IProductView& product) {
        // Extract product info
        ProductInfo prod_info = product.GetProductInfo();
        std::cout << "  - " << prod_info.name 
                  << " (" << prod_info.sku << ")\n";
        std::cout << "    Price: $" << prod_info.price << "\n";
        std::cout << "    Stock: " << prod_info.stock << " units\n";
      });
      
      std::cout << "\n";
    });
  }
  
  void UpdatePrices(double multiplier) {
    ICatalogView catalog = m_provider.LoadCatalog();
    
    // Navigate hierarchy
    catalog.ForEachCategory([multiplier](const ICategoryView& category) {
      category.ForEachProduct([multiplier](const IProductView& product) {
        // Extract data
        ProductInfo info = product.GetProductInfo();
        
        // Update price
        info.price *= multiplier;
        
        // Save back (provider handles this)
        // SaveProduct(info);
        
        std::cout << "Updated " << info.sku 
                  << " to $" << info.price << "\n";
      });
    });
  }
  
  void FindProduct(const std::string& sku) {
    ICatalogView catalog = m_provider.LoadCatalog();
    
    catalog.ForEachCategory([&sku](const ICategoryView& category) {
      category.ForEachProduct([&sku](const IProductView& product) {
        if (product.GetSKU() == sku) {
          ProductInfo info = product.GetProductInfo();
          std::cout << "Found: " << info.name << "\n";
          std::cout << "Price: $" << info.price << "\n";
          std::cout << "Stock: " << info.stock << "\n";
        }
      });
    });
  }
};

//------------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------------

int main() {
  // Create provider (JSON in this case)
  JsonCatalogProvider json_provider;
  
  // Create application (no JSON dependency!)
  CatalogManager manager(json_provider);
  
  // Use application
  manager.LoadAndDisplay();
  manager.UpdatePrices(1.1);  // 10% price increase
  manager.FindProduct("SKU-12345");
  
  // Could swap to XML provider without changing application code!
  // XmlCatalogProvider xml_provider;
  // CatalogManager manager(xml_provider);
  
  return 0;
}
```

**Key Points**:
- ✅ Application code has ZERO JSON dependency
- ✅ Can swap to XML, FlatBuffers, database, etc.
- ✅ No wasteful copying of catalog data
- ✅ Direct access via views

---

## Interfacing with Running Game

### Challenge: Game Loop Integration

How do we use the view pattern when the game is already running?

### Solution: Lazy Loading with View Caching

```cpp
//==============================================================================
// Game Integration Pattern
//==============================================================================

class GameStateManager {
private:
  ICatalogProvider& m_catalog_provider;
  
  // Cache the view (lightweight, just pointers)
  std::optional<ICatalogView> m_cached_catalog_view;
  
  // Game objects (heavy, contains actual game state)
  std::vector<CategoryUI> m_category_uis;
  std::vector<ProductObject> m_product_objects;
  
public:
  GameStateManager(ICatalogProvider& provider) 
      : m_catalog_provider(provider) {}
  
  //----------------------------------------------------------------------------
  // Pattern 1: Load Once at Startup
  //----------------------------------------------------------------------------
  
  void Initialize() {
    // Load catalog view once
    m_cached_catalog_view = m_catalog_provider.LoadCatalog();
    
    // Configure game objects from view
    ConfigureGameObjects(*m_cached_catalog_view);
    
    // View stays cached, data accessible anytime
  }
  
  void ConfigureGameObjects(const ICatalogView& catalog) {
    // Clear existing
    m_category_uis.clear();
    m_product_objects.clear();
    
    // Create UI for each category
    catalog.ForEachCategory([this](const ICategoryView& cat_view) {
      CategoryInfo info = cat_view.GetCategoryInfo();
      
      // Create UI object
      CategoryUI ui;
      ui.name = info.name;
      ui.description = info.description;
      
      // Create product objects
      cat_view.ForEachProduct([this, &ui](const IProductView& prod_view) {
        ProductInfo prod_info = prod_view.GetProductInfo();
        
        ProductObject obj;
        obj.sku = prod_info.sku;
        obj.name = prod_info.name;
        obj.price = prod_info.price;
        obj.stock = prod_info.stock;
        
        m_product_objects.push_back(obj);
        ui.product_skus.push_back(obj.sku);
      });
      
      m_category_uis.push_back(ui);
    });
  }
  
  //----------------------------------------------------------------------------
  // Pattern 2: Reload During Gameplay
  //----------------------------------------------------------------------------
  
  void ReloadCatalog() {
    // Player action: "Refresh Catalog"
    
    // Load new view
    m_cached_catalog_view = m_catalog_provider.LoadCatalog();
    
    // Reconfigure game objects
    ConfigureGameObjects(*m_cached_catalog_view);
    
    // UI will update on next render
  }
  
  //----------------------------------------------------------------------------
  // Pattern 3: On-Demand Loading
  //----------------------------------------------------------------------------
  
  ProductObject* GetProductBySKU(const std::string& sku) {
    // Check if already loaded
    for (auto& obj : m_product_objects) {
      if (obj.sku == sku) {
        return &obj;
      }
    }
    
    // Not loaded - fetch from view on demand
    if (!m_cached_catalog_view) {
      m_cached_catalog_view = m_catalog_provider.LoadCatalog();
    }
    
    // Search in view
    ProductObject* found = nullptr;
    m_cached_catalog_view->ForEachCategory([&](const ICategoryView& cat) {
      cat.ForEachProduct([&](const IProductView& prod) {
        if (prod.GetSKU() == sku) {
          ProductInfo info = prod.GetProductInfo();
          
          // Create game object
          ProductObject obj;
          obj.sku = info.sku;
          obj.name = info.name;
          obj.price = info.price;
          obj.stock = info.stock;
          
          m_product_objects.push_back(obj);
          found = &m_product_objects.back();
        }
      });
    });
    
    return found;
  }
  
  //----------------------------------------------------------------------------
  // Pattern 4: Incremental Loading
  //----------------------------------------------------------------------------
  
  void LoadCategoryIncrementally(const std::string& category_id) {
    if (!m_cached_catalog_view) {
      m_cached_catalog_view = m_catalog_provider.LoadCatalog();
    }
    
    // Find category by ID
    auto cat_view = m_cached_catalog_view->GetCategoryByID(category_id);
    
    // Load only this category's products
    CategoryUI ui;
    CategoryInfo info = cat_view.GetCategoryInfo();
    ui.name = info.name;
    ui.description = info.description;
    
    cat_view.ForEachProduct([this, &ui](const IProductView& prod) {
      ProductInfo prod_info = prod.GetProductInfo();
      
      ProductObject obj;
      obj.sku = prod_info.sku;
      obj.name = prod_info.name;
      obj.price = prod_info.price;
      obj.stock = prod_info.stock;
      
      m_product_objects.push_back(obj);
      ui.product_skus.push_back(obj.sku);
    });
    
    m_category_uis.push_back(ui);
  }
  
  //----------------------------------------------------------------------------
  // Game Loop Update
  //----------------------------------------------------------------------------
  
  void Update(float delta_time) {
    // Game objects update independently
    for (auto& product : m_product_objects) {
      product.Update(delta_time);
    }
    
    // View is passive - just data access
    // No need to "update" the view
  }
  
  void Render() {
    // Render UI from game objects
    for (const auto& ui : m_category_uis) {
      RenderCategoryUI(ui);
    }
    
    for (const auto& product : m_product_objects) {
      RenderProduct(product);
    }
  }
  
  //----------------------------------------------------------------------------
  // Save Current State
  //----------------------------------------------------------------------------
  
  void SaveCurrentState() {
    // Capture current game object state
    CatalogData catalog_data;
    catalog_data.info.name = "My Catalog";
    catalog_data.info.version = "1.0";
    catalog_data.info.currency = "USD";
    
    for (const auto& ui : m_category_uis) {
      CategoryData cat_data;
      cat_data.info.id = ui.id;
      cat_data.info.name = ui.name;
      cat_data.info.description = ui.description;
      
      for (const auto& sku : ui.product_skus) {
        ProductObject* obj = FindProductBySKU(sku);
        if (obj) {
          ProductData prod_data;
          prod_data.info.sku = obj->sku;
          prod_data.info.name = obj->name;
          prod_data.info.price = obj->price;
          prod_data.info.stock = obj->stock;
          
          cat_data.products.push_back(prod_data);
        }
      }
      
      catalog_data.categories.push_back(cat_data);
    }
    
    // Provider handles serialization
    m_catalog_provider.SaveCatalog(catalog_data);
  }
};
```

### Integration Timeline

```
Game Startup
────────────
1. Create Provider                 → Knows how to load data
2. Create GameStateManager          → Uses provider
3. gameState.Initialize()           → Loads view, configures objects
4. Game objects ready               → Can start game loop

Game Loop
─────────
5. Update()                         → Game objects update
6. Render()                         → Game objects render
   [View is passive - no updates needed]

Player Action: "Refresh Data"
──────────────────────────────
7. gameState.ReloadCatalog()        → Load new view
8. ConfigureGameObjects()           → Update game objects
9. Continue game loop               → New data active

On-Demand Load: "Show Product X"
─────────────────────────────────
10. gameState.GetProductBySKU(x)    → Check cache
11. If not loaded → Query view      → Load from provider
12. Create game object              → Add to cache
13. Return object                   → Use in game

Game Save
─────────
14. gameState.SaveCurrentState()    → Capture game objects
15. Provider.SaveCatalog(data)      → Serialize to format
16. Write to file/database          → Persist
```

### Key Integration Principles

1. **Views are Passive** - They provide data access, they don't update
2. **Game Objects are Active** - They have state, behavior, update logic
3. **Views → Objects** - Views configure objects, not used directly in game loop
4. **Cache Views** - Keep view around for on-demand access
5. **Provider Owns Data** - Provider ensures data lifetime during view usage

---

## Summary

### ASCII Diagrams Provided

1. **Hierarchical View Pattern Overview** - Full system architecture
2. **Data Flow During Load** - Step-by-step loading process
3. **Comparison with Alternatives** - Why view pattern is better

### Generic Example

- **Domain**: Product catalog (not game-specific)
- **Structure**: Catalog → Categories → Products
- **Complete implementation** of view interfaces and JSON provider
- **Application code** with zero format dependency

### Game Integration

- **Pattern 1**: Load once at startup
- **Pattern 2**: Reload during gameplay
- **Pattern 3**: On-demand loading
- **Pattern 4**: Incremental loading
- **Timeline**: Startup → Loop → Actions → Save

### Key Takeaway

Views are **data accessors**, not game objects. They configure game objects, then game objects run independently.

```
View Pattern = Data Access Layer
Game Objects = Active Game State

Load:   View → Configure → Game Object
Update: Game Object (view not involved)
Save:   Game Object → Capture → Provider
```

---

**See Also**:
- [Save/Load Workflow](SAVE_LOAD_WORKFLOW_ANALYSIS.md) - Game-specific examples
- [Configuration vs Data Structs](CONFIGURATION_VS_DATA_STRUCTS_ANALYSIS.md) - View pattern deep dive
- [Current State Analysis](CURRENT_STATE_ANALYSIS_2025.md) - Context

---

**Document Complete**: December 5, 2025
