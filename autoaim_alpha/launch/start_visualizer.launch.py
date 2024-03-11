
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

package_name = 'autoaim_alpha'

def generate_launch_description():
    return LaunchDescription(
            Node(
            package=package_name,
            node_executable='node_visualizer',
            node_name='node_visualizer',
            output='screen'
        )
    )


