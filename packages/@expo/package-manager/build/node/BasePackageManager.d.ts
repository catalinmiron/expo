import { SpawnPromise, SpawnResult } from '@expo/spawn-async';
import { PackageManager, PackageManagerOptions } from '../PackageManager';
import { PendingSpawnPromise } from '../utils/spawn';
export declare abstract class BasePackageManager implements PackageManager {
    readonly silent: boolean;
    readonly logger: (...args: any) => void;
    readonly options: PackageManagerOptions;
    constructor({ silent, logger, ...options }?: PackageManagerOptions);
    /** Get the name of the package manager */
    abstract readonly name: string;
    /** Get the executable binary of the package manager */
    abstract readonly bin: string;
    /** Get the lockfile for this package manager */
    abstract readonly lockFile: string;
    abstract addAsync(namesOrFlags: string[]): SpawnPromise<SpawnResult> | PendingSpawnPromise<SpawnResult>;
    abstract addDevAsync(namesOrFlags: string[]): SpawnPromise<SpawnResult> | PendingSpawnPromise<SpawnResult>;
    abstract addGlobalAsync(namesOrFlags: string[]): SpawnPromise<SpawnResult> | PendingSpawnPromise<SpawnResult>;
    abstract removeAsync(namesOrFlags: string[]): SpawnPromise<SpawnResult> | PendingSpawnPromise<SpawnResult>;
    abstract removeDevAsync(namesOrFlags: string[]): SpawnPromise<SpawnResult> | PendingSpawnPromise<SpawnResult>;
    abstract removeGlobalAsync(namesOrFlags: string[]): SpawnPromise<SpawnResult> | PendingSpawnPromise<SpawnResult>;
    abstract workspaceRoot(): PackageManager | null;
    /** Ensure the CWD is set to a non-empty string */
    protected ensureCwdDefined(method?: string): string;
    runAsync(command: string[]): SpawnPromise<SpawnResult>;
    versionAsync(): Promise<string>;
    configAsync(key: string): Promise<string>;
    removeLockfileAsync(): Promise<void>;
    installAsync(flags?: string[]): SpawnPromise<SpawnResult> | PendingSpawnPromise<SpawnResult>;
    uninstallAsync(): Promise<void>;
}
