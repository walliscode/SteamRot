# AssetManager Workflow Diagrams

This document provides detailed workflow diagrams for the AssetManager, showing both current state and proposed refactored state using Mermaid diagrams.

**Date:** 2025-12-24  
**Related:** ASSET_MANAGER_ANALYSIS.md

---

## Table of Contents

1. [Current State Workflows](#current-state-workflows)
2. [Proposed State Workflows](#proposed-state-workflows)
3. [Component Interaction Diagrams](#component-interaction-diagrams)
4. [Data Flow Diagrams](#data-flow-diagrams)

---

## Current State Workflows

### Current Workflow 1: Engine Startup and Default Asset Loading

```mermaid
sequenceDiagram
    participant Engine
    participant EngineResources
    participant AssetManager
    participant DataAccessFactory
    participant IAssetDataProvider
    participant FileSystem
    participant UIStyleProvider

    Engine->>EngineResources: Create
    EngineResources->>DataAccessFactory: Create
    EngineResources->>AssetManager: Create(data_access_factory)
    
    Engine->>AssetManager: LoadDefaultAssets()
    
    rect rgb(200, 220, 240)
        note right of AssetManager: Font Loading Phase
        AssetManager->>DataAccessFactory: GetAssetDataProvider()
        DataAccessFactory-->>AssetManager: IAssetDataProvider*
        AssetManager->>IAssetDataProvider: LoadAssetData()
        IAssetDataProvider-->>AssetManager: AssetData (fonts, ui_styles)
        
        loop For each font in AssetData
            AssetManager->>AssetManager: AddFont(font_name)
            AssetManager->>FileSystem: Read font file (.ttf)
            FileSystem-->>AssetManager: Font data
            AssetManager->>AssetManager: Store in m_fonts map
        end
    end
    
    rect rgb(240, 220, 200)
        note right of AssetManager: UI Style Loading Phase
        AssetManager->>AssetManager: LoadUIStyles()
        AssetManager->>UIStyleProvider: Create(m_fonts)
        note right of UIStyleProvider: Direct map passing!
        AssetManager->>UIStyleProvider: ProvideUIStyles()
        UIStyleProvider->>UIStyleProvider: Load FlatBuffers data
        UIStyleProvider->>UIStyleProvider: Configure styles
        UIStyleProvider-->>AssetManager: vector<UIStyle>
        AssetManager->>AssetManager: Store in m_ui_styles map
    end
    
    AssetManager-->>Engine: Success/Failure
```

### Current Workflow 2: Scene Change Asset Loading

```mermaid
sequenceDiagram
    participant SceneManager
    participant AssetManager
    participant DataAccessFactory
    participant IAssetDataProvider

    SceneManager->>AssetManager: LoadSceneAssets(scene_type)
    
    AssetManager->>DataAccessFactory: GetAssetDataProvider()
    DataAccessFactory-->>AssetManager: IAssetDataProvider*
    
    AssetManager->>IAssetDataProvider: LoadSceneAssetData(scene_type)
    IAssetDataProvider-->>AssetManager: AssetData
    
    rect rgb(255, 200, 200)
        note right of AssetManager: PROBLEM: Does nothing with data!
        AssetManager->>AssetManager: return success (no-op)
    end
    
    AssetManager-->>SceneManager: Success (but nothing loaded!)
```

### Current Workflow 3: Font Access Pattern

```mermaid
sequenceDiagram
    participant Consumer as Logic/System/Configurator
    participant IFontProvider
    participant AssetManager

    Consumer->>IFontProvider: GetFont("font_name")
    note right of IFontProvider: Consumer uses interface
    
    IFontProvider->>AssetManager: GetFont("font_name")
    note right of AssetManager: AssetManager implements IFontProvider
    
    AssetManager->>AssetManager: Search m_fonts map
    
    alt Font found
        AssetManager-->>IFontProvider: shared_ptr<Font>
        IFontProvider-->>Consumer: shared_ptr<Font>
    else Font not found
        AssetManager-->>IFontProvider: FailInfo
        IFontProvider-->>Consumer: FailInfo
    end
```

### Current Workflow 4: UI Style Configuration (Detailed)

```mermaid
sequenceDiagram
    participant AssetManager
    participant UIStyleProvider as FlatbuffersUIStyleDataProvider
    participant DataLoader as FlatbuffersDataLoader
    participant FileSystem
    participant FontMap as m_fonts (Direct Access!)

    AssetManager->>AssetManager: LoadUIStyles()
    
    rect rgb(255, 220, 220)
        note right of AssetManager: PROBLEM: Direct map passing
        AssetManager->>UIStyleProvider: Create(m_fonts)
        note right of UIStyleProvider: Provider has direct reference!
    end
    
    AssetManager->>UIStyleProvider: ProvideUIStyles()
    
    UIStyleProvider->>DataLoader: Create
    UIStyleProvider->>DataLoader: ProvideUIStylesData()
    DataLoader->>FileSystem: Read FlatBuffers file
    FileSystem-->>DataLoader: UIStyleData
    DataLoader-->>UIStyleProvider: vector<UIStyleData*>
    
    loop For each style in vector
        UIStyleProvider->>UIStyleProvider: ConfigureBaseStyle()
        UIStyleProvider->>UIStyleProvider: ConfigurePanelStyle()
        UIStyleProvider->>UIStyleProvider: ConfigureButtonStyle()
        
        rect rgb(255, 220, 220)
            note right of UIStyleProvider: PROBLEM: Direct map access
            UIStyleProvider->>FontMap: Find font in map
            FontMap-->>UIStyleProvider: shared_ptr<Font>
        end
        
        UIStyleProvider->>UIStyleProvider: ConfigureDropDownStyles()
    end
    
    UIStyleProvider-->>AssetManager: vector<UIStyle>
    AssetManager->>AssetManager: Store in m_ui_styles map
```

---

## Proposed State Workflows

### Proposed Workflow 1: Engine Startup and Default Asset Loading

```mermaid
sequenceDiagram
    participant Engine
    participant EngineResources
    participant AssetManager
    participant DataAccessFactory
    participant IFontLoader
    participant FontLoader
    participant IStyleLoader
    participant StyleLoader
    participant AssetRegistry

    Engine->>EngineResources: Create
    EngineResources->>DataAccessFactory: Create
    EngineResources->>AssetManager: Create(data_access_factory)
    AssetManager->>AssetRegistry: Create
    
    Engine->>AssetManager: LoadDefaultAssets()
    
    rect rgb(200, 240, 200)
        note right of AssetManager: Font Loading Phase (Improved)
        AssetManager->>DataAccessFactory: GetFontLoader()
        DataAccessFactory-->>AssetManager: IFontLoader*
        AssetManager->>IFontLoader: LoadDefaultFonts()
        
        IFontLoader->>FontLoader: LoadDefaultFonts()
        FontLoader->>FontLoader: Get asset configuration
        
        loop For each font
            FontLoader->>FontLoader: Load from filesystem
            FontLoader->>FontLoader: Configure font properties
        end
        
        FontLoader-->>IFontLoader: vector<Font>
        IFontLoader-->>AssetManager: vector<Font>
        AssetManager->>AssetRegistry: StoreFonts(fonts)
    end
    
    rect rgb(200, 220, 240)
        note right of AssetManager: Style Loading Phase (Improved)
        AssetManager->>DataAccessFactory: GetStyleLoader()
        DataAccessFactory-->>AssetManager: IStyleLoader*
        AssetManager->>IStyleLoader: LoadDefaultStyles(this as IFontProvider)
        note right of IStyleLoader: Passes interface, not map!
        
        IStyleLoader->>StyleLoader: LoadDefaultStyles(IFontProvider)
        StyleLoader->>StyleLoader: Load FlatBuffers data
        
        loop For each style
            StyleLoader->>StyleLoader: Configure style
            StyleLoader->>AssetManager: GetFont() via IFontProvider
            note right of StyleLoader: Uses interface method
            AssetManager-->>StyleLoader: shared_ptr<Font>
        end
        
        StyleLoader-->>IStyleLoader: vector<UIStyle>
        IStyleLoader-->>AssetManager: vector<UIStyle>
        AssetManager->>AssetRegistry: StoreStyles(styles)
    end
    
    AssetManager-->>Engine: Success/Failure
```

### Proposed Workflow 2: Scene Change Asset Loading

```mermaid
sequenceDiagram
    participant SceneManager
    participant AssetManager
    participant DataAccessFactory
    participant IFontLoader
    participant IStyleLoader
    participant AssetRegistry

    SceneManager->>AssetManager: LoadSceneAssets(scene_type)
    
    rect rgb(200, 240, 200)
        note right of AssetManager: Scene Font Loading (if any)
        AssetManager->>DataAccessFactory: GetFontLoader()
        DataAccessFactory-->>AssetManager: IFontLoader*
        AssetManager->>IFontLoader: LoadSceneFonts(scene_type)
        IFontLoader-->>AssetManager: vector<Font>
        AssetManager->>AssetRegistry: MergeFonts(fonts)
    end
    
    rect rgb(200, 220, 240)
        note right of AssetManager: Scene Style Loading (if any)
        AssetManager->>DataAccessFactory: GetStyleLoader()
        DataAccessFactory-->>AssetManager: IStyleLoader*
        AssetManager->>IStyleLoader: LoadSceneStyles(scene_type, IFontProvider)
        IStyleLoader-->>AssetManager: vector<UIStyle>
        AssetManager->>AssetRegistry: MergeStyles(styles)
    end
    
    rect rgb(240, 240, 200)
        note right of AssetManager: Future: Texture Loading
        AssetManager->>DataAccessFactory: GetTextureLoader()
        DataAccessFactory-->>AssetManager: ITextureLoader*
        AssetManager->>ITextureLoader: LoadSceneTextures(scene_type)
        ITextureLoader-->>AssetManager: vector<Texture>
        AssetManager->>AssetRegistry: MergeTextures(textures)
    end
    
    AssetManager-->>SceneManager: Success/Failure
```

### Proposed Workflow 3: Font Access Pattern (Unchanged)

```mermaid
sequenceDiagram
    participant Consumer as Logic/System/Configurator
    participant IFontProvider
    participant AssetManager
    participant AssetRegistry

    Consumer->>IFontProvider: GetFont("font_name")
    IFontProvider->>AssetManager: GetFont("font_name")
    AssetManager->>AssetRegistry: GetFont("font_name")
    
    alt Font found
        AssetRegistry-->>AssetManager: shared_ptr<Font>
        AssetManager-->>IFontProvider: shared_ptr<Font>
        IFontProvider-->>Consumer: shared_ptr<Font>
    else Font not found
        AssetRegistry-->>AssetManager: nullopt
        AssetManager-->>IFontProvider: FailInfo
        IFontProvider-->>Consumer: FailInfo
    end
```

### Proposed Workflow 4: UI Style Configuration (Improved)

```mermaid
sequenceDiagram
    participant AssetManager
    participant IStyleLoader
    participant StyleLoader
    participant DataLoader as FlatbuffersDataLoader
    participant IFontProvider
    participant FileSystem

    AssetManager->>AssetManager: LoadUIStyles()
    
    AssetManager->>IStyleLoader: LoadDefaultStyles(this as IFontProvider)
    note right of IStyleLoader: Passes interface only!
    
    IStyleLoader->>StyleLoader: LoadDefaultStyles(IFontProvider)
    
    StyleLoader->>DataLoader: Create
    StyleLoader->>DataLoader: ProvideUIStylesData()
    DataLoader->>FileSystem: Read FlatBuffers file
    FileSystem-->>DataLoader: UIStyleData
    DataLoader-->>StyleLoader: vector<UIStyleData*>
    
    loop For each style in vector
        StyleLoader->>StyleLoader: ConfigureBaseStyle()
        StyleLoader->>StyleLoader: ConfigurePanelStyle()
        StyleLoader->>StyleLoader: ConfigureButtonStyle()
        
        rect rgb(200, 240, 200)
            note right of StyleLoader: IMPROVED: Uses interface
            StyleLoader->>IFontProvider: GetFont("font_name")
            IFontProvider->>AssetManager: GetFont("font_name")
            AssetManager-->>IFontProvider: shared_ptr<Font>
            IFontProvider-->>StyleLoader: shared_ptr<Font>
        end
        
        StyleLoader->>StyleLoader: ConfigureDropDownStyles()
    end
    
    StyleLoader-->>IStyleLoader: vector<UIStyle>
    IStyleLoader-->>AssetManager: vector<UIStyle>
    AssetManager->>AssetManager: Store styles
```

---

## Component Interaction Diagrams

### Current State Component Interactions

```mermaid
graph TB
    subgraph "Current Architecture"
        AssetManager[AssetManager<br/>- m_fonts map<br/>- m_ui_styles map<br/>- LoadDefaultAssets<br/>- LoadSceneAssets<br/>- AddFont<br/>- LoadUIStyles]
        
        DataAccessFactory[DataAccessFactory]
        IAssetDataProvider[IAssetDataProvider<br/>Interface]
        FlatbuffersAssetDP[FlatbuffersAssetDataProvider]
        
        UIStyleProvider[FlatbuffersUIStyleDataProvider<br/>Direct access to m_fonts!]
        IFontProvider[IFontProvider<br/>Interface]
        
        FileSystem[FileSystem<br/>Font files]
        
        AssetManager -->|creates directly| UIStyleProvider
        AssetManager -.->|passes m_fonts<br/>PROBLEM!| UIStyleProvider
        AssetManager -->|implements| IFontProvider
        AssetManager -->|uses| DataAccessFactory
        AssetManager -->|reads from| FileSystem
        
        DataAccessFactory -->|provides| IAssetDataProvider
        IAssetDataProvider <|.. FlatbuffersAssetDP
        
        UIStyleProvider -->|accesses directly<br/>PROBLEM!| AssetManager
    end
    
    style UIStyleProvider fill:#ffcccc
    style AssetManager fill:#ffffcc
```

### Proposed State Component Interactions

```mermaid
graph TB
    subgraph "Proposed Architecture"
        AssetManager[AssetManager<br/>- AssetRegistry member<br/>- LoadDefaultAssets<br/>- LoadSceneAssets<br/>- Coordinates loaders]
        
        AssetRegistry[AssetRegistry<br/>- m_fonts map<br/>- m_ui_styles map<br/>- m_textures map<br/>- Add/Get methods]
        
        DataAccessFactory[DataAccessFactory]
        
        IFontLoader[IFontLoader<br/>Interface]
        FontLoader[FontLoader<br/>File I/O]
        
        IStyleLoader[IStyleLoader<br/>Interface]
        StyleLoader[StyleLoader<br/>Configuration]
        
        IFontProvider[IFontProvider<br/>Interface]
        
        FileSystem[FileSystem]
        
        AssetManager -->|owns| AssetRegistry
        AssetManager -->|uses| DataAccessFactory
        AssetManager -->|implements| IFontProvider
        AssetManager -->|gets loaders from| DataAccessFactory
        
        DataAccessFactory -->|provides| IFontLoader
        DataAccessFactory -->|provides| IStyleLoader
        
        IFontLoader <|.. FontLoader
        IStyleLoader <|.. StyleLoader
        
        FontLoader -->|reads from| FileSystem
        StyleLoader -.->|uses interface| IFontProvider
        IFontProvider <|.. AssetManager
        
        AssetManager -->|delegates storage| AssetRegistry
    end
    
    style AssetManager fill:#ccffcc
    style AssetRegistry fill:#ccffcc
    style StyleLoader fill:#ccffcc
```

---

## Data Flow Diagrams

### Current State Data Flow

```mermaid
flowchart TD
    Start([Engine Startup])
    
    Start --> LoadDefault[AssetManager::LoadDefaultAssets]
    
    LoadDefault --> GetProvider[Get IAssetDataProvider<br/>from DataAccessFactory]
    GetProvider --> LoadConfig[LoadAssetData]
    LoadConfig --> FontList[AssetData<br/>fonts: vector of names]
    
    FontList --> FontLoop{For each font}
    FontLoop -->|iterate| AddFont[AddFont]
    AddFont --> ReadFile[Read .ttf from filesystem]
    ReadFile --> CreateFont[Create sf::Font]
    CreateFont --> StoreFontMap[Store in m_fonts map]
    StoreFontMap --> FontLoop
    
    FontLoop -->|done| LoadStyles[LoadUIStyles]
    
    LoadStyles --> CreateProvider[Create FlatbuffersUIStyleDataProvider<br/>Pass m_fonts DIRECTLY]
    CreateProvider --> ProvideStyles[ProvideUIStyles]
    ProvideStyles --> LoadFB[Load FlatBuffers data]
    LoadFB --> StyleLoop{For each style}
    
    StyleLoop -->|iterate| ConfigStyle[Configure style]
    ConfigStyle --> AccessMap[Access m_fonts map<br/>DIRECTLY]
    AccessMap --> GetFont[Get font pointer]
    GetFont --> StyleLoop
    
    StyleLoop -->|done| StoreStyles[Store in m_ui_styles map]
    StoreStyles --> End([Complete])
    
    style CreateProvider fill:#ffcccc
    style AccessMap fill:#ffcccc
```

### Proposed State Data Flow

```mermaid
flowchart TD
    Start([Engine Startup])
    
    Start --> LoadDefault[AssetManager::LoadDefaultAssets]
    
    LoadDefault --> GetFontLoader[Get IFontLoader<br/>from DataAccessFactory]
    GetFontLoader --> LoadFonts[LoadDefaultFonts]
    LoadFonts --> FontLoader[FontLoader reads files]
    FontLoader --> FontVec[vector of Fonts]
    FontVec --> StoreFonts[AssetRegistry::StoreFonts]
    
    StoreFonts --> GetStyleLoader[Get IStyleLoader<br/>from DataAccessFactory]
    GetStyleLoader --> LoadStyles[LoadDefaultStyles<br/>Pass IFontProvider interface]
    LoadStyles --> StyleLoader[StyleLoader loads data]
    StyleLoader --> ConfigLoop{For each style}
    
    ConfigLoop -->|iterate| ConfigStyle[Configure style]
    ConfigStyle --> UseInterface[Call IFontProvider::GetFont<br/>via INTERFACE]
    UseInterface --> ReturnFont[AssetManager returns font]
    ReturnFont --> ConfigLoop
    
    ConfigLoop -->|done| StyleVec[vector of UIStyles]
    StyleVec --> StoreStyles[AssetRegistry::StoreStyles]
    StoreStyles --> End([Complete])
    
    style UseInterface fill:#ccffcc
    style LoadStyles fill:#ccffcc
```

### Scene Asset Loading Data Flow (Proposed)

```mermaid
flowchart TD
    Start([SceneManager::ChangeScene])
    
    Start --> LoadScene[AssetManager::LoadSceneAssets]
    
    LoadScene --> CheckFonts{Scene has<br/>custom fonts?}
    CheckFonts -->|yes| GetFontLoader[Get IFontLoader]
    GetFontLoader --> LoadSceneFonts[LoadSceneFonts]
    LoadSceneFonts --> MergeFonts[AssetRegistry::MergeFonts]
    CheckFonts -->|no| CheckStyles
    
    MergeFonts --> CheckStyles{Scene has<br/>custom styles?}
    CheckStyles -->|yes| GetStyleLoader[Get IStyleLoader]
    GetStyleLoader --> LoadSceneStyles[LoadSceneStyles]
    LoadSceneStyles --> MergeStyles[AssetRegistry::MergeStyles]
    CheckStyles -->|no| CheckTextures
    
    MergeStyles --> CheckTextures{Scene has<br/>textures?}
    CheckTextures -->|yes| GetTextureLoader[Get ITextureLoader]
    GetTextureLoader --> LoadTextures[LoadSceneTextures]
    LoadTextures --> MergeTextures[AssetRegistry::MergeTextures]
    CheckTextures -->|no| End
    
    MergeTextures --> End([Complete])
    
    style MergeFonts fill:#ccffcc
    style MergeStyles fill:#ccffcc
    style MergeTextures fill:#ccffcc
```

---

## Comparison: Before vs After

### Responsibility Separation

**Before:**
```
AssetManager = Registry + Loading + Configuration + Access
```

**After:**
```
AssetManager = Coordination + Access
AssetRegistry = Storage
FontLoader = Font I/O
StyleLoader = Style Configuration
```

### Coupling Reduction

**Before:**
- AssetManager → FlatbuffersUIStyleDataProvider (tight coupling)
- FlatbuffersUIStyleDataProvider → m_fonts (encapsulation violation)

**After:**
- AssetManager → IStyleLoader (loose coupling via interface)
- StyleLoader → IFontProvider (loose coupling via interface)

### Testability Improvement

**Before:**
- Must test AssetManager with real files
- Cannot test style loading without AssetManager
- Hard to mock font access

**After:**
- Can test AssetManager with mock loaders
- Can test StyleLoader with mock IFontProvider
- Easy to provide test fonts without files

---

## Summary

### Current State Problems (Visualized)

1. **Tight Coupling:** Direct instantiation and map passing
2. **Mixed Responsibilities:** Registry + Loading + Configuration in one class
3. **Encapsulation Violation:** Internal map passed to external component
4. **Incomplete Features:** Scene loading does nothing

### Proposed State Benefits (Visualized)

1. **Loose Coupling:** Interface-based dependencies
2. **Clear Responsibilities:** Separate classes for storage, loading, configuration
3. **Proper Encapsulation:** Only interfaces exposed
4. **Complete Features:** Scene loading functional

### Migration Path

Phase 1-2: Create new components (non-breaking)  
Phase 3-4: Refactor existing code (controlled breaking)  
Phase 5-6: Complete features and fix bugs

---

## Document Metadata

**Created:** 2025-12-24  
**Author:** GitHub Copilot Agent  
**Format:** Mermaid diagrams (GitHub compatible)  
**Related:** ASSET_MANAGER_ANALYSIS.md  
**Status:** Complete  
**Review:** Pending
